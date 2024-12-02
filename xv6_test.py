import pathlib
import subprocess
import abc
import sys
import enum
import time
import os
import json
import psutil
import shutil
import signal

XV6_TIMEOUT_SEC = 30
DEFAULT_XV6_DIR = "."
DEFAULT_GDB_PORT = 26005

class CaptureType(enum.Enum):
    FULL = 1
    STDOUT = 2

class Xv6Error(Exception):
    def __init__(self, message, stdout, stderr):
        self.stdout = stdout
        self.stderr = stderr
        super().__init__(f"{message}:\nstdout:\n{stdout.decode()}\nstderr:\n{stderr.decode()}")

class Xv6BuildFailed(Xv6Error):
    def __init__(self, stdout, stderr):
        super().__init__("Build failed", stdout, stderr)

class Xv6Timeout(Exception):
    def __init__(self, stdout, stderr):
        super().__init__("Timeout expired", stdout, stderr)


class Xv6Runner:
    def __init__(self, xv6_dir=None):
        if xv6_dir is None:
            if len(sys.argv) > 1:
                self.xv6_dir = pathlib.Path(sys.argv[1])
            else:
                self.xv6_dir = pathlib.Path(DEFAULT_XV6_DIR)
        if len(sys.argv) > 2:
            self.gdb_port = int(sys.argv[2])
        else:
            self.gdb_port = DEFAULT_GDB_PORT
        self.shinit_file = self.xv6_dir / '.shinit'
        self.fs_file = self.xv6_dir / 'fs.img'
        self.fscat_exe = self.xv6_dir / 'mkfs/fscat'
        
    def run_xv6_exe(self, name, capture_type=CaptureType.STDOUT,
                    extra_uprogs=[], 
                    rebuild_fs_beginning=True, 
                    compile_xv6=True,
                    rebuild_fs_end=True):
        # Remove fs.img to force a rebuild that includes .shinit
        if rebuild_fs_beginning:
            self.fs_file.unlink(missing_ok=True)

        if capture_type == CaptureType.STDOUT:
            self.shinit_file.write_text(f'{name} > stdout.txt\nhalt')
        else:
            self.shinit_file.write_text(f'{name}\nhalt')

        shinit_relative_path = self.shinit_file.relative_to(self.xv6_dir)
        extra_uprogs = ' '.join(f'user/_{name}' for name in extra_uprogs)

        try:
            if compile_xv6:
                proc = self.make(
                    target='all',
                    capture_output=True,
                    EXTRA_FS_FILES=shinit_relative_path,
                    EXTRA_UPROGS=extra_uprogs
                )
                (stdout, stderr) = proc.communicate()
                if proc.returncode != 0:
                    raise Xv6BuildFailed(stdout, stderr)

            target = 'qemu'
            make_args = {}
            timeout = False
            try:
                proc = self.make(
                    target=target, 
                    capture_output=True, 
                    EXTRA_FS_FILES=shinit_relative_path,
                    EXTRA_UPROGS=extra_uprogs,
                    **make_args
                )

                (stdout, stderr) = proc.communicate(timeout=XV6_TIMEOUT_SEC)
                if proc.returncode != 0:
                    raise Xv6BuildFailed(stdout, stderr)

            except subprocess.TimeoutExpired:
                print('timeout! killing xv6...', flush=True)
                os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
                print('killed xv6')
                (stdout, stderr) = proc.communicate()
                raise Xv6Timeout(stdout, stderr)

            if capture_type == CaptureType.STDOUT:
                stdout = self.fscat('stdout.txt')
            else:
                stdout = stdout.decode()

            return stdout

        finally:
            # Remove fs files to force a rebuild the next time xv6 is booted
            if rebuild_fs_end:
                self.fs_file.unlink(missing_ok=True)
                self.shinit_file.unlink(missing_ok=True)

    def fscat(self, file_name):
        fscat_exe = self.fscat_exe.relative_to(self.xv6_dir)
        fs_file = self.fs_file.relative_to(self.xv6_dir)
        proc = self.make(fscat_exe)
        code = proc.wait()
        assert code == 0

        proc = self.run(
            [fscat_exe, fs_file, file_name],
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE,
            text=True
        )
        (stdout, stderr) = proc.communicate()
        print(stdout)
        print(stderr)
        assert proc.returncode == 0

        return stdout

    def make(self, target=None, capture_output=False, **kwargs):
        cmd = ['make']
        cmd += [f'{k}={v}' for k, v in kwargs.items()]

        if target is not None:
            cmd.append(target)

        if capture_output:
            return self.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        else:
            return self.run(cmd)

    def run(self, cmd, **kwargs):
        return subprocess.Popen(cmd, cwd=self.xv6_dir, start_new_session=True, **kwargs)


class TestContext(abc.ABC):
    def __init__(self, xv6_runner=Xv6Runner()):
        self.xv6_runner = xv6_runner
        self.failed = False
        self.grade = {}

    def _test_success(self, name):
        print(f'\033[92m{name}: ok\033[0m')

    def _test_failure(self, name, msg):
        print(f'\033[91m{name}: failed')
        print(f"{msg}\033[0m")
        print("")
        self.failed = True

    @abc.abstractmethod
    def run_tests(self):
        pass

    def run(self):
        self.run_tests()

        if self.failed:
            print('Some tests failed')
            sys.exit(1)
        else:
            print('All tests passed')
