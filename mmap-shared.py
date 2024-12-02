#!/usr/bin/env python3

import pathlib
import re

import sys

from xv6_test import Xv6Runner, TestContext, CaptureType, Xv6Error

pattern_mapping = r'(0x[0-9a-fA-F]{16}) -> (0x[0-9a-fA-F]{16}),\smode=([US]),\sperms=([r-][w-][x-])'
mem1 = 0x0000000baddad000
mem2 = 0x0000000bddddd000

def _contains_mapping(lines, target_va, target_mode, target_perms):
    target_va_str = f'0x{target_va:016x}'
    for line in lines:
        match_mapping = re.search(pattern_mapping, line)
        if match_mapping:
            va, pa, mode, perms = match_mapping.groups()
            if va == target_va_str:
                good_mode = True
                good_perms = True
                if target_mode is not None:
                    good_mode = target_mode == mode
                if target_perms is not None:
                    good_perms = target_perms == perms
                return good_mode and good_perms
    return False

def _get_mappings_subset(lines, pattern_beg, pattern_end):
    mappings = []
    begin = False
    for line in lines:
        match_mapping = re.search(pattern_mapping, line)
        match_beg = re.search(pattern_beg, line)
        match_end = re.search(pattern_end, line)
        if match_beg:
            begin = True
        elif begin and match_mapping:
            mappings.append(line)
        elif match_end:
            return mappings
    return mappings

def _get_counters_subset(lines, pattern_beg, pattern_end):
    inside_match = False
    text = ""

    for line in lines:
        match_beg = re.search(pattern_beg, line)
        if match_beg:
            inside_match = True
            continue

        if inside_match:
            match_end = re.search(pattern_end, line)
            if match_end:
                break
            text += line + ";"
    
    pattern = r"(\d+)\s*=\s*(\d+)"
    matches = re.findall(pattern, text)
    result = {int(key): int(value) for key, value in matches}
    return result

class TestMmapShared(TestContext):

    def run_tests(self):
        self._check_output_error("mmap syscall implemented", "test_mmap1")

        def _test_mmap2_mappings(stdout, errors):
            lines = stdout.splitlines()
            mappings = _get_mappings_subset(lines, "Mappings", "SUCCESS")
            if not _contains_mapping(mappings, mem1, 'U', 'rw-'):
                errors.append(f"missing mapping at 0x{mem1:016x} (or permissions wrong)")
            if not _contains_mapping(mappings, mem2, 'U', 'rw-'):
                errors.append(f"missing mapping at 0x{mem2:016x} (or permissions wrong)")

        self._check_output_error("add new mappings", "test_mmap2", _test_mmap2_mappings)
        self._check_output_error("does not map same virtual address twice", "test_mmap3", _test_mmap2_mappings)

        def _test_permissions(stdout, errors):
            lines = stdout.splitlines()
            mappings = _get_mappings_subset(lines, "Mappings", "SUCCESS")
            if not _contains_mapping(mappings, mem1, 'U', 'rw-'):
                errors.append(f"missing mapping at 0x{mem1:016x} (or permissions wrong)")
            if not _contains_mapping(mappings, mem1 + 0x1000, 'U', 'r--'):
                errors.append(f"missing mapping at 0x{mem1 + 0x1000:016x} (or permissions wrong)")
            if not _contains_mapping(mappings, mem1 + 0x2000, 'U', '-w-'):
                errors.append(f"missing mapping at 0x{mem1 + 0x2000:016x} (or permissions wrong)")
            if not _contains_mapping(mappings, mem1 + 0x3000, 'U', 'r-x'):
                errors.append(f"missing mapping at 0x{mem1 + 0x3000:016x} (or permissions wrong)")
        self._check_output_error("check permissions", "test_mmap4", _test_permissions)

        self._check_output_error("mmap fails gracefully when mapping more than 10 pages", "test_mmap5")
        self._check_output_error("mappings transfered to child", "test_mmap6", _test_mmap2_mappings)

        self._check_output_error("private pages are private", "test_mmap7")
        self._check_output_error("shared pages are shared", "test_mmap8")

        def _test_counter_init(stdout, errors):
            counters = _get_counters_subset(stdout.splitlines(), "Counters", "SUCCESS")
            if len(counters) < 1:
                errors.append(f"counters not dumped")
                return
            if counters[0] != 1:
                errors.append(f"expected first counter to be 1")

        self._check_output_error("initialize shared page counter to 1", "test_mmap9", _test_counter_init)

        def _test_counter_increment(stdout, errors):
            counters = _get_counters_subset(stdout.splitlines(), "Counters", "SUCCESS")
            if len(counters) < 2:
                errors.append(f"counters not dumped")
                return
            if counters[0] != 2:
                errors.append(f"expected first counter to be 2")
            if counters[1] != 2:
                errors.append(f"expected second counter to be 2")

        self._check_output_error("increment shared page counter", "test_mmap10", _test_counter_increment)

        def _test_counter_decrement(stdout, errors):
            counters = _get_counters_subset(stdout.splitlines(), "In child", "In parent")
            if len(counters) < 1:
                errors.append(f"counters not dumped")
                return
            if counters[0] != 2:
                errors.append(f"expected first counter to be 2")
            counters = _get_counters_subset(stdout.splitlines(), "In parent", "SUCCESS")
            if len(counters) < 1:
                errors.append(f"counters not dumped")
                return
            if counters[0] != 1:
                errors.append(f"expected first counter to be 1")

        self._check_output_error("decrement shared page counter", "test_mmap11", _test_counter_decrement)

        def _test_counter_error(stdout, errors):
            counters = _get_counters_subset(stdout.splitlines(), "Counters", "SUCCESS")
            if len(counters) < 20:
                errors.append(f"counters not dumped")
                return
            if counters[9] != 1:
                errors.append(f"expected 10th counter to be 1")
            if counters[10] != 0:
                errors.append(f"expected 11th counter to be 0")

        self._check_output_error("handles errors correctly", "test_mmap12", _test_counter_error)
        

    def _check_output_error(self, name, program, extra_check=None):
        stdout = None
        try:
            stdout = self.xv6_runner.run_xv6_exe(
                program,
                extra_uprogs=[program],
                capture_type=CaptureType.FULL
            )
        except Xv6Error as e:
            self._test_failure(name, str(e))
            return

        success = False
        errors = []
        for line in stdout.splitlines():
            if re.search(r'trap', line):
                success = False
                errors.append(line.rstrip())
            if re.search(r'SUCCESS', line):
                success = True
            if re.search(r'Error:', line):
                success = False
                errors.append(line.rstrip())

        if extra_check:
            extra_check(stdout, errors)
                
        if success and len(errors) == 0:
            self._test_success(name)
        else:
            self._test_failure(name, '; '.join(errors) + f'\n\nxv6 output below:\n{stdout}')
        return success
    

if __name__ == '__main__':
    TestMmapShared().run()
