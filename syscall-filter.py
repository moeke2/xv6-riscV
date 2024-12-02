#!/usr/bin/env python3
import pathlib
import re
import sys
import subprocess
from xv6_test import Xv6Runner, TestContext, CaptureType, Xv6Error

DEBUG=0

class TestSystemCalls(TestContext):

    def get_stdout(self, program):
        try:
            stdout = self.xv6_runner.run_xv6_exe(
                program,
                extra_uprogs=[program],
                capture_type=CaptureType.FULL,
                compile_xv6=False,
            )
        except Xv6Error as e:
            self._test_failure(program, str(e))
            return
        if stdout == '':
            msg = 'No output from test. Did you create the evaluation ' \
                  'executable? See the terminal output above for more info.'
            self._test_failure(program, msg)
            return
        if DEBUG:
            print(f'=== Running {program}')
            print(f'---stdout---\n{stdout}\n---end stdout---')
        return stdout.splitlines()


    def run_tests(self):
        self._test_syscallfilter_initialized()
        self._test_syscallfilter_blocks()
        self._test_syscallfilter_killed()
        self._test_syscallfilter_child()
        self._test_syscallfilter_retval()


    """ Initialize filter correctly """
    def _test_syscallfilter_initialized(self):
        name = "filter is correctly initialized"
        stdout = self.get_stdout('testfilter4')
        success = 0
        for line in stdout:
            match = re.search(r"init_syscall_filter\s*=\s*(\d+)", line)
            if match:
                result = int(match.group(1))
                if result == 3:
                    success = True
        if success:
            self._test_success(name)
        else:
            self._test_failure(name, "filter not initialized\n\nxv6 output below:\n" + "\n".join(stdout))


    """ Blocks syscalls """
    def _test_syscallfilter_blocks(self):
        name = "syscalls are correctly blocked"
        stdout = self.get_stdout('testfilter1')
        match1 = 0
        match2 = 0
        for line in stdout:
            if re.search(r"\[testfilter\] Allowed", line):
                match1 = 1
            if re.search(r"\[testfilter\] Not allowed - parent", line):
                match2 = 1

        if match1 and not match2:
            self._test_success(name)
        else:
            self._test_failure(name, "syscalls not blocked\n\nxv6 output below:\n" + "\n".join(stdout))


    """ Kill process after blocking """
    def _test_syscallfilter_killed(self):
        name = "process is killed after bad syscall"
        stdout = self.get_stdout('testfilter5')
        match1 = 0
        match2 = 0
        for line in stdout:
            if re.search(r"\[testfilter\] Allowed", line):
                match1 = 1
            if re.search(r"\[testfilter\] Error", line):
                match2 = 1

        if match1 and not match2:
            self._test_success(name)
        else:
            self._test_failure(name, "process not killed on blocked syscall\n\nxv6 output below:\n" + "\n".join(stdout))


    """ Transfer filter to child """
    def _test_syscallfilter_child(self):
        name = "child inherits filter"
        stdout = self.get_stdout('testfilter1')
        match1 = 0
        match2 = 0
        for line in stdout:
            if re.search(r"\[testfilter\] Allowed", line):
                match1 = 1
            if re.search(r"\[testfilter\] Not allowed - child", line):
                match2 = 1

        if match1 and not match2:
            self._test_success(name)
        else:
            self._test_failure(name, "syscall filter not inherited for children\n\nxv6 output below:\n" + "\n".join(stdout))


    """ Test return values """
    def _test_syscallfilter_retval(self):
        name = "return values are handled properly"
        stdout = self.get_stdout('testfilter3')
        success = 0
        for line in stdout:
            match = re.search(r"\[testfilter\] syscallfilter\(0\) = -1", line)
            if match:
                success += 1
            match = re.search(r"\[testfilter\] syscallfilter\(45\) = -1", line)
            if match:
                success += 1
            match = re.search(r"\[testfilter\] syscallfilter\(1\) = 0", line)
            if match:
                success += 1
        if success == 3:
            self._test_success(name)
        else:
            self._test_failure(name, "system call does not correctly check argument system call number range\n\nxv6 output below:\n" + "\n".join(stdout))

if __name__ == '__main__':
    TestSystemCalls().run()
