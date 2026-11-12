"""Validate manifest chainloading and compiler environment compatibility."""

import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
if str(ROOT / "scripts") not in sys.path:
    sys.path.insert(0, str(ROOT / "scripts"))


class PresetContracts(unittest.TestCase):
    def test_manifest_chainloads_compiler(self):
        from cli.preset import get_preset_cache_variable, get_preset_compiler

        self.assertIsNone(get_preset_cache_variable("clang-glibc-release", "CMAKE_CXX_COMPILER"))
        self.assertIn("vcpkg.cmake", get_preset_cache_variable("clang-glibc-release", "CMAKE_TOOLCHAIN_FILE"))
        self.assertTrue(get_preset_compiler("clang-glibc-release").endswith("/clang-glibc/bin/clang++"))
        self.assertEqual(
            get_preset_cache_variable("gcc-glibc-asan-ubsan", "VCPKG_TARGET_TRIPLET"),
            "x64-linux-gcc-glibc-asan",
        )

    def test_build_environment_chainloads_sdk(self):
        import os
        from unittest import mock

        from cli.utils import prepare_build_environment

        with tempfile.TemporaryDirectory() as temporary:
            with mock.patch.dict(os.environ, {"URING_TOOLCHAIN_ROOT": "/opt/sdk"}):
                env = prepare_build_environment(Path(temporary))
            self.assertEqual(env["TOOLCHAIN_ROOT"], "/opt/sdk")
            for key in ("TMPDIR", "TMP", "TEMP"):
                self.assertEqual(env[key], str(Path(temporary) / "tmp"))
