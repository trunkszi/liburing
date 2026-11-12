"""Exercise the launcher and preset selection through their public interfaces."""

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
if str(ROOT / "scripts") not in sys.path:
    sys.path.insert(0, str(ROOT / "scripts"))
for venv_site in (ROOT / "scripts/cli/.venv/lib").glob("python*/site-packages"):
    if str(venv_site) not in sys.path:
        sys.path.insert(0, str(venv_site))


class CliContracts(unittest.TestCase):
    def test_launcher_help(self):
        result = subprocess.run([ROOT / "uring", "--help"], check=False, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        for command in ("build", "test", "format", "tidy", "verify", "bench"):
            self.assertIn(command, result.stdout)

    def test_all_twelve_presets(self):
        from cli.preset import resolve_preset

        for compiler in ("clang-glibc", "gcc-glibc", "clang-musl", "gcc-musl"):
            self.assertEqual(resolve_preset(compiler=compiler), compiler + "-release")
            self.assertEqual(resolve_preset(compiler=compiler, debug=True), compiler + "-debug")
            self.assertEqual(resolve_preset(compiler=compiler, dev=True), compiler + "-asan-ubsan")

    def test_json_invalid_preset(self):
        result = subprocess.run(
            [ROOT / "uring", "--format", "json", "build", "--preset", "absent-preset"],
            check=False,
            capture_output=True,
            text=True,
        )
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(json.loads(result.stdout)["success"])

    def test_fragment_discovery(self):
        from unittest import mock

        from cli import utils

        with tempfile.TemporaryDirectory(prefix="uring-fragments-") as temporary:
            root = Path(temporary)
            (root / "CMakeLists.txt").write_text(
                "cmake_minimum_required(VERSION 3.30)\n"
                "project(fragment LANGUAGES NONE)\n"
                "enable_testing()\n"
                "add_subdirectory(src)\n"
                "add_subdirectory(tests)\n"
                "add_subdirectory(benches)\n"
            )
            (root / "src").mkdir()
            (root / "src/CMakeLists.txt").write_text("add_custom_target(fragment_library)\n")
            (root / "tests").mkdir()
            (root / "tests/CMakeLists.txt").write_text("add_subdirectory(unit)\n")
            (root / "tests/unit").mkdir()
            (root / "tests/unit/CMakeLists.txt").write_text(
                'add_test(NAME fragment_case COMMAND "${CMAKE_COMMAND}" -E true)\n'
            )
            (root / "benches").mkdir()
            (root / "benches/CMakeLists.txt").write_text("add_custom_target(fragment_bench)\n")
            build = root / "build"
            result = subprocess.run(
                ["cmake", "-S", str(root), "-B", str(build), "-G", "Ninja"],
                check=False,
                capture_output=True,
                text=True,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            with mock.patch.object(utils, "PROJECT_ROOT", root):
                self.assertFalse(utils.should_reconfigure(build))
                nested = root / "tests" / "extra" / "CMakeLists.txt"
                nested.parent.mkdir()
                nested.write_text("add_custom_target(fragment_watched)\n")
                self.assertTrue(utils.should_reconfigure(build))
            (root / "tests/unit/CMakeLists.txt").write_text(
                'add_test(NAME fragment_case COMMAND "${CMAKE_COMMAND}" -E true)\n'
                "add_custom_target(fragment_late)\n"
            )
            result = subprocess.run(
                ["cmake", "--build", str(build), "--target", "fragment_library"],
                check=False,
                capture_output=True,
                text=True,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            targets = subprocess.check_output(["ninja", "-C", str(build), "-t", "targets", "all"], text=True)
            self.assertIn("fragment_late:", targets)
            self.assertIn("fragment_bench:", targets)
            self.assertIn("fragment_library:", targets)
            cases = json.loads(
                subprocess.check_output(["ctest", "--test-dir", str(build), "--show-only=json-v1"], text=True)
            )
            self.assertIn("fragment_case", [case["name"] for case in cases["tests"]])

    def test_should_reconfigure_ignores_generated_lists(self):
        import os
        from unittest import mock

        from cli import utils

        with tempfile.TemporaryDirectory(prefix="uring-reconfigure-") as temporary:
            root = Path(temporary)
            past = 1_700_000_000
            (root / "CMakePresets.json").write_text("{}\n")
            (root / "src").mkdir()
            (root / "tests").mkdir()
            (root / "src/CMakeLists.txt").write_text("add_library(fragment INTERFACE)\n")
            (root / "tests/CMakeLists.txt").write_text("enable_testing()\n")
            build = root / "build" / "preset"
            build.mkdir(parents=True)
            cache = build / "CMakeCache.txt"
            cache.write_text("CMAKE_BUILD_TYPE:STRING=Release\n")
            (build / "build.ninja").write_text("# ninja\n")
            for path in (
                root / "CMakePresets.json",
                root / "src/CMakeLists.txt",
                root / "tests/CMakeLists.txt",
            ):
                os.utime(path, (past, past))
            generated = (
                build / "CMakeLists.txt",
                root / ".venv" / "lib" / "CMakeLists.txt",
                root / ".git" / "modules" / "CMakeLists.txt",
            )
            for path in generated:
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text("project(generated LANGUAGES NONE)\n")
                os.utime(path, (past + 100, past + 100))
            os.utime(cache, (past + 50, past + 50))
            with mock.patch.object(utils, "PROJECT_ROOT", root):
                self.assertFalse(utils.should_reconfigure(build))
                os.utime(root / "src/CMakeLists.txt", (past + 80, past + 80))
                self.assertTrue(utils.should_reconfigure(build))
                os.utime(root / "src/CMakeLists.txt", (past, past))
                os.utime(cache, (past + 90, past + 90))
                self.assertFalse(utils.should_reconfigure(build))
                os.utime(root / "tests/CMakeLists.txt", (past + 120, past + 120))
                self.assertTrue(utils.should_reconfigure(build))

    def test_expand_preset_value_rewrites_home(self):
        import os
        from unittest import mock

        from cli.preset import _expand_preset_value, get_preset_compiler

        with mock.patch.dict(os.environ, {"HOME": "/home/builder"}):
            expanded = _expand_preset_value("$penv{HOME}/.local/toolchain/clang-glibc/bin/clang++")
            self.assertEqual(expanded, "/home/builder/.local/toolchain/clang-glibc/bin/clang++")
            self.assertEqual(_expand_preset_value("$env{HOME}/bin"), "/home/builder/bin")
        compiler = get_preset_compiler("clang-glibc-release")
        self.assertIsNotNone(compiler)
        self.assertNotIn("$penv{", compiler)
        self.assertNotIn("$env{", compiler)
        self.assertTrue(compiler.endswith("/clang-glibc/bin/clang++"))

    def test_clang_glibc_cfg_is_relocatable(self):
        with tempfile.TemporaryDirectory(prefix="uring-cfg-") as temporary:
            sdk = Path(temporary) / "clang-glibc"
            (sdk / "bin").mkdir(parents=True)
            script = Path(temporary) / "gen.cmake"
            toolchain = ROOT / "cmake" / "UringToolchain.cmake"
            script.write_text(f'include("{toolchain}")\n_uring_ensure_clang_glibc_cfg("{sdk}")\n')
            result = subprocess.run(["cmake", "-P", str(script)], check=False, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            for name in ("clang.cfg", "clang++.cfg"):
                text = (sdk / "bin" / name).read_text()
                self.assertIn("<CFGDIR>", text)
                self.assertNotIn(str(Path.home()), text)

    def test_package_config_defaults_module_scan_off(self):
        config = (ROOT / "cmake" / "uring-config.cmake.in").read_text()
        init_at = config.index("@PACKAGE_INIT@")
        guard_at = config.index("if(NOT DEFINED CMAKE_CXX_SCAN_FOR_MODULES)")
        self.assertLess(init_at, guard_at)
        self.assertIn("set(CMAKE_CXX_SCAN_FOR_MODULES OFF)", config)
        self.assertIn("find_dependency(uarch CONFIG REQUIRED)", config)
        with tempfile.TemporaryDirectory(prefix="uring-scan-") as temporary:
            script = Path(temporary) / "scan.cmake"
            script.write_text(
                "if(NOT DEFINED CMAKE_CXX_SCAN_FOR_MODULES)\n"
                "  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)\n"
                "endif()\n"
                'if(NOT CMAKE_CXX_SCAN_FOR_MODULES STREQUAL "OFF")\n'
                '  message(FATAL_ERROR "module scan default is not OFF")\n'
                "endif()\n"
                "set(CMAKE_CXX_SCAN_FOR_MODULES ON)\n"
                "if(NOT DEFINED CMAKE_CXX_SCAN_FOR_MODULES)\n"
                "  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)\n"
                "endif()\n"
                'if(NOT CMAKE_CXX_SCAN_FOR_MODULES STREQUAL "ON")\n'
                '  message(FATAL_ERROR "explicit module scan was overwritten")\n'
                "endif()\n"
            )
            result = subprocess.run(["cmake", "-P", str(script)], check=False, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_find_clang_tool_override_and_versioned_fallback(self):
        from unittest import mock

        from cli import utils

        with tempfile.TemporaryDirectory(prefix="uring-tool-") as temporary:
            custom_tool = Path(temporary) / "custom-clang-tidy"
            custom_tool.write_text("#!/bin/sh\nexit 0\n")
            custom_tool.chmod(0o755)
            with mock.patch.dict("os.environ", {"URING_CLANG_TIDY_BIN": str(custom_tool)}, clear=False):
                self.assertEqual(utils.find_clang_tool("clang-tidy"), str(custom_tool))

        def fake_which(candidate):
            if candidate == "clang-tidy-23":
                return "/usr/bin/clang-tidy-23"
            if candidate == "clang-tidy":
                return "/usr/bin/clang-tidy"
            return None

        with (
            mock.patch.dict("os.environ", {}, clear=True),
            mock.patch.object(utils.os, "access", return_value=False),
            mock.patch.object(utils.shutil, "which", side_effect=fake_which),
        ):
            self.assertEqual(utils.find_clang_tool("clang-tidy"), "/usr/bin/clang-tidy-23")

    def test_verify_audit_probe_objects_detects_clean_and_forbidden_symbols(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_verify

        with tempfile.TemporaryDirectory(prefix="uring-verify-") as temporary:
            build = Path(temporary)
            probe_dir = build / "tests/CMakeFiles/native_smoke_probe.dir/unit"
            probe_dir.mkdir(parents=True)
            probe_obj = probe_dir / "smoke_test.cc.o"
            probe_obj.write_bytes(b"\x7fELF\x02\x01\x01\x00")

            tables = []
            infos = []
            out = SimpleNamespace(
                table=lambda title, cols, rows: tables.append((title, cols, rows)),
                info=lambda msg: infos.append(msg),
            )

            def clean_capture(cmd, cwd=None):
                tool = Path(cmd[0]).name
                if tool == "llvm-nm":
                    return "abi_cqe_echo T 0 10\n"
                if tool == "llvm-readobj":
                    return "Section {\n  Name: .text\n}\n"
                if tool == "llvm-objdump":
                    return "0000000000000000 <abi_cqe_echo>:\n  0: xor eax, eax\n  2: ret\n"
                if tool == "llvm-size":
                    return "   text    data     bss     dec     hex filename\n     16       0       0      16      10 smoke_test.cc.o\n"
                raise AssertionError(f"Unexpected tool: {tool}")

            with (
                mock.patch.object(cmd_verify, "_resolve_llvm_tool", side_effect=lambda name: f"/bin/{name}"),
                mock.patch.object(cmd_verify, "capture", side_effect=clean_capture),
            ):
                summaries = cmd_verify.audit_probe_objects(build, out, gate="lint")
                self.assertEqual(len(summaries), 1)
                self.assertEqual(summaries[0]["text"], "16")
                self.assertTrue(tables)
                self.assertTrue(infos)

            def tainted_capture(cmd, cwd=None):
                tool = Path(cmd[0]).name
                if tool == "llvm-nm":
                    return "__asan_report_load8 U\nmain T 0 10\n"
                return clean_capture(cmd, cwd=cwd)

            with (
                mock.patch.object(cmd_verify, "_resolve_llvm_tool", side_effect=lambda name: f"/bin/{name}"),
                mock.patch.object(cmd_verify, "capture", side_effect=tainted_capture),
                self.assertRaisesRegex(RuntimeError, "Forbidden symbol '__asan'"),
            ):
                cmd_verify.audit_probe_objects(build, out, gate="lint")

    def test_verify_requires_complete_abi_contract(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_verify
        with tempfile.TemporaryDirectory() as temporary:
            obj = Path(temporary) / "probe.o"
            obj.write_bytes(b"\x7fELF")
            def output(command):
                tool = Path(command[0]).name
                if tool == "llvm-nm":
                    return "abi_cqe_echo T 0 1\n"
                if tool == "llvm-objdump":
                    return "<abi_cqe_echo>:\n  ret\n"
                if tool == "llvm-size":
                    return "text data bss dec\n1 0 0 1\n"
                return ""
            with (
                mock.patch.object(cmd_verify, "discover_probe_objects", return_value=[obj]),
                mock.patch.object(cmd_verify, "_resolve_llvm_tool", side_effect=lambda name: name),
                mock.patch.object(cmd_verify, "capture", side_effect=output),
                self.assertRaisesRegex(RuntimeError, "Missing contracted ABI probes"),
            ):
                cmd_verify.audit_probe_objects(
                    Path(temporary), SimpleNamespace(table=mock.Mock(), info=mock.Mock()), gate="abi-check"
                )

    def test_verify_mca_rejects_symbol_budget_regression(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_verify
        metrics = "Instructions: 100\nTotal Cycles: 100\nTotal uOps: 100\nBlock RThroughput: 1000.0\n"
        with (
            mock.patch.object(cmd_verify, "_resolve_llvm_tool", return_value="llvm-mca"),
            mock.patch.object(cmd_verify.subprocess, "run", return_value=SimpleNamespace(stdout=metrics)),
            self.assertRaisesRegex(RuntimeError, "MCA budget exceeded.*abi_cqe_echo"),
        ):
            cmd_verify.audit_mca({"abi_cqe_echo": ["ret"]}, SimpleNamespace(table=mock.Mock()), "probe.o")

    def test_verify_gate_specific_instruction_checks(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_verify

        out = SimpleNamespace(table=mock.Mock(), info=mock.Mock())
        clean = {"abi_cqe_echo": ["mov rax, rdi", "ret"]}
        cmd_verify.audit_abi(clean, out, "probe.o")
        cmd_verify.audit_instructions(clean, out, "probe.o")
        for instructions in (["push rbp", "ret"], ["sub rsp, 16", "ret"],
                             ["mov qword ptr [rsp - 8], rax", "ret"]):
            with self.assertRaisesRegex(RuntimeError, "Stack"):
                cmd_verify.audit_abi({"abi_cqe_echo": instructions}, out, "probe.o")
        for name, instructions, diagnostic in (
            ("arbitrary_probe", ["idiv rcx"], "division"),
            ("abi_cqe_echo", ["call rax"], "Indirect"),
            ("proactor_static_dispatch_probe", ["call qword ptr [rax]"], "Indirect"),
        ):
            with self.assertRaisesRegex(RuntimeError, diagnostic):
                cmd_verify.audit_instructions({name: instructions}, out, "probe.o")
        with mock.patch.object(cmd_verify, "audit_mca") as mca:
            for gate in ("abi-check", "asm-check", "mca-gate", "all"):
                out.table.reset_mock()
                cmd_verify.audit_disassembly(clean, out, "probe.o", gate)
                titles = [call.args[0] for call in out.table.call_args_list]
                self.assertEqual(any("ABI" in title for title in titles), gate in ("abi-check", "all"))
                self.assertEqual(any("Instruction" in title for title in titles), gate in ("asm-check", "all"))
            self.assertEqual(mca.call_count, 2)

    def test_verify_mca_executes_and_validates_metrics(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_verify

        functions = {"abi_cqe_echo": ["mov rax, rdi", "jne 0x20 <abi_cqe_echo+0x20>", "ret"]}
        metrics = "Instructions: 300\nTotal Cycles: 105\nTotal uOps: 300\nBlock RThroughput: 1.0\n"
        out = SimpleNamespace(table=mock.Mock())
        with (mock.patch.object(cmd_verify, "_resolve_llvm_tool", return_value="llvm-mca"),
              mock.patch.object(cmd_verify.subprocess, "run", return_value=SimpleNamespace(stdout=metrics)) as run):
            cmd_verify.audit_mca(functions, out, "probe.o")
            self.assertEqual([call.args[0][-1] for call in run.call_args_list],
                             [f"--mcpu={cpu}" for cpu in ("znver4", "znver5", "sapphirerapids")])
            self.assertIn(".intel_syntax noprefix", run.call_args.kwargs["input"])
            self.assertIn("jne .Lmca_target", run.call_args.kwargs["input"])
            self.assertTrue(run.call_args.kwargs["check"])
            run.return_value.stdout = metrics.replace("1.0", "0.0")
            with self.assertRaisesRegex(RuntimeError, "throughput"):
                cmd_verify.audit_mca(functions, out, "probe.o")
            run.return_value.stdout = "Instructions: 300\n"
            with self.assertRaisesRegex(RuntimeError, "metric"):
                cmd_verify.audit_mca(functions, out, "probe.o")

    def test_verify_mca_checks_every_cpu_budget(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_verify
        metrics = "Instructions: 100\nTotal Cycles: 100\nTotal uOps: 100\nBlock RThroughput: 1.0\n"
        def result(command, **kwargs):
            return SimpleNamespace(stdout=metrics.replace("1.0", "1000.0") if command[-1] == "--mcpu=znver5" else metrics, stderr="")
        with (
            mock.patch.object(cmd_verify, "_resolve_llvm_tool", return_value="llvm-mca"),
            mock.patch.object(cmd_verify.subprocess, "run", side_effect=result),
            self.assertRaisesRegex(RuntimeError, "budget exceeded.*znver5"),
        ):
            cmd_verify.audit_mca({"abi_cqe_echo": ["ret"]}, SimpleNamespace(table=mock.Mock()), "probe.o")

    def test_verify_mca_uring_symbol_budgets(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_verify

        symbols = (
            ("_ZN5uring4caps10KernelCaps5ProbeINS0_18NativeProbeBackendEEESt8expectedIS1_NS_14UringErrorCodeEERKNS0_12RequiredCapsE", 384.0),
            ("_ZN5uring4caps10KernelCaps5ProbeINS0_18NativeProbeBackendEEENSt3__18expectedIS1_NS_14UringErrorCodeEEERKNS0_12RequiredCapsE", 384.0),
            ("_ZN5uring4caps19VerifyKernelVersionERKNS0_12RequiredCapsE", 128.0),
            ("_ZN5uring4caps18ParseKernelVersionEPKc", 32.0),
            ("_ZN5uring3sys5PanicEPKc", 64.0),
            ("unclassified_probe", 24.0),
        )
        for symbol, budget in symbols:
            with self.subTest(symbol=symbol):
                out = SimpleNamespace(table=mock.Mock())
                with mock.patch.object(cmd_verify, "_mca_metrics") as metrics:
                    metrics.return_value = (["100", "100", "100", str(budget)], "znver4")
                    cmd_verify.audit_mca({symbol: ["ret"]}, out, "probe.o")
                    metrics.return_value = (["100", "100", "100", str(budget + 0.1)], "znver4")
                    with self.assertRaisesRegex(RuntimeError, "MCA budget exceeded"):
                        cmd_verify.audit_mca({symbol: ["ret"]}, out, "probe.o")

    def test_verify_mca_gcc_advance_baseline(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_verify
        metrics = "Instructions: 100\nTotal Cycles: 100\nTotal uOps: 100\nBlock RThroughput: 25.5\n"
        with (mock.patch.object(cmd_verify, "_resolve_llvm_tool", return_value="llvm-mca"),
              mock.patch.object(cmd_verify.subprocess, "run", return_value=SimpleNamespace(stdout=metrics, stderr=""))):
            out = SimpleNamespace(table=mock.Mock())
            with self.assertRaisesRegex(RuntimeError, "budget exceeded.*t2_advance"):
                cmd_verify.audit_mca({"t2_advance": ["ret"]}, out, "probe.o")
            cmd_verify.audit_mca({"t2_advance": ["ret"]}, out, "probe.o", gcc=True)

    def test_verify_mca_stub_cpu_fallback(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_verify
        metrics = "Instructions: 100\nTotal Cycles: 100\nTotal uOps: 100\nBlock RThroughput: 1.0\n"
        def result(command, **kwargs):
            if command[-1] != "--mcpu=native":
                raise subprocess.CalledProcessError(1, command, stderr="unknown mcpu target in synthetic stub")
            return SimpleNamespace(stdout=metrics, stderr="")
        with (mock.patch.object(cmd_verify, "_resolve_llvm_tool", return_value="llvm-mca"),
              mock.patch.object(cmd_verify.subprocess, "run", side_effect=result)):
            cmd_verify.audit_mca({"abi_cqe_echo": ["ret"]}, SimpleNamespace(table=mock.Mock()), "probe.o")

    def test_verify_tier2_cli_exits_nonzero(self):
        for gate in ("pmu", "gem5"):
            with self.subTest(gate=gate):
                result = subprocess.run([ROOT / "uring", "verify", gate, "--preset", "clang-glibc-release"],
                                        check=False, capture_output=True, text=True)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("not implemented in static probe mode", " ".join((result.stdout + result.stderr).split()))

    def test_verify_disassembly_extraction(self):
        from cli import cmd_verify
        raw = "0000000000000000 <abi_cqe_echo>:\n  0: 48 89 f8 mov rax, rdi\n  3: c3 ret\n"
        plain = "<abi_cqe_echo>:\n  mov rax, rdi\n  ret\n"
        expected = {"abi_cqe_echo": ["mov rax, rdi", "ret"]}
        self.assertEqual(cmd_verify.parse_disassembly(raw), expected)
        self.assertEqual(cmd_verify.parse_disassembly(plain), expected)
        trampoline = "<wrapper>:\n  jmp 0x5 <wrapper+0x5>\n\tR_X86_64_PLT32 target-0x4\n"
        self.assertEqual(cmd_verify.parse_disassembly(trampoline), {"wrapper": ["jmp target"]})
        with self.assertRaisesRegex(RuntimeError, "No instructions"):
            cmd_verify.parse_disassembly("garbage")

    def test_verify_tier2_requires_runner(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_verify

        with (mock.patch.object(cmd_verify, "ensure_configured", return_value=Path("/tmp/build")),
              mock.patch.object(cmd_verify, "list_ninja_targets", return_value={}),
              mock.patch.object(cmd_verify, "build_targets") as build):
            for gate in ("pmu", "gem5"):
                with self.assertRaisesRegex(ValueError, "Tier 2 gate.*not implemented in static probe mode"):
                    cmd_verify.verify_cmd(SimpleNamespace(obj=None), gate=gate, debug=False, dev=False,
                                          compiler="clang", sanitizer="asan-ubsan",
                                          preset="clang-glibc-release", verbose=False)
            build.assert_not_called()

    def test_tidy_filters_project_cpp_and_flags_non_clang_diagnostics(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_tidy

        with tempfile.TemporaryDirectory(prefix="uring-tidy-") as temporary:
            repo = Path(temporary) / "repo"
            external = Path(temporary) / "external"
            build = repo / "build/clang-glibc-release"
            (repo / "tests/unit").mkdir(parents=True)
            (repo / "tests/compile_fail").mkdir(parents=True)
            external.mkdir(parents=True)
            build.mkdir(parents=True)

            good_cpp = repo / "tests/unit/smoke_test.cc"
            good_cpp.write_text("int main() { return 0; }\n")
            compile_fail_cpp = repo / "tests/compile_fail/bad.cc"
            compile_fail_cpp.write_text("int main() { return 0; }\n")
            external_cpp = external / "cpuid.cc"
            external_cpp.write_text("int main() { return 0; }\n")
            build_c = build / "mixed_consumer.c"
            build_c.write_text("int main(void) { return 0; }\n")

            db = [
                {"directory": str(build), "file": str(p), "arguments": ["clang++", "-freflection-latest", "-c", str(p)]}
                for p in (good_cpp, compile_fail_cpp, external_cpp, build_c)
            ]
            (build / "compile_commands.json").write_text(json.dumps(db))

            captured_commands = []

            async def fake_check_all(commands, jobs, out, *, p2996_capable=True):
                captured_commands.extend(commands)

            out = SimpleNamespace(fmt="json", emit=lambda *a, **k: None, console=SimpleNamespace(print=lambda *a, **k: None))
            with (
                mock.patch.object(cmd_tidy, "PROJECT_ROOT", repo),
                mock.patch.object(cmd_tidy, "find_clang_tool", return_value="/usr/bin/clang-tidy-23"),
                mock.patch.object(cmd_tidy, "_verify_tidy_config", return_value=None),
                mock.patch.object(cmd_tidy, "supports_reflection", return_value=False),
                mock.patch.object(cmd_tidy, "ensure_configured", return_value=build),
                mock.patch.object(cmd_tidy, "_check_all", side_effect=fake_check_all),
            ):
                cmd_tidy.tidy_cmd(
                    SimpleNamespace(obj=out),
                    paths=None,
                    debug=False,
                    dev=False,
                    compiler="clang",
                    sanitizer="asan-ubsan",
                    preset="clang-glibc-release",
                    jobs=1,
                    verbose=False,
                )

            self.assertEqual(len(captured_commands), 1)
            self.assertEqual(captured_commands[0][1], str(good_cpp.resolve()))
            self.assertIn("--extra-arg=-D__is_consteval_only(...)=false", captured_commands[0])

            sample_output = (
                "tests/unit/smoke_test.cc:3:10: error: 'meta' file not found [clang-diagnostic-error]\n"
                "tests/unit/smoke_test.cc:12:8: warning: invalid case style for struct 'bad_name' [readability-identifier-naming]\n"
            )
            findings = cmd_tidy._extract_non_clang_diagnostics(sample_output)
            self.assertEqual(len(findings), 1)
            self.assertIn("[readability-identifier-naming]", findings[0])

    def test_tidy_fatal_errors_always_fail(self):
        import asyncio
        from types import SimpleNamespace

        from cli import cmd_tidy

        diagnostic = "fixture.cc:1:10: fatal error: 'missing.h' file not found [clang-diagnostic-error]\n"
        for capable in (False, True):
            for status in (0, 1):
                with self.subTest(capable=capable, status=status):
                    events = []
                    out = SimpleNamespace(fmt="json", emit=lambda *a, sink=events, **k: sink.append((a, k)))
                    command = [sys.executable, "-c", f"import sys; sys.stderr.write({diagnostic!r}); sys.exit({status})"]
                    with self.assertRaisesRegex(RuntimeError, "clang-tidy checks failed"):
                        asyncio.run(cmd_tidy._check_all([command], 1, out, p2996_capable=capable))
                    self.assertEqual(events[0][1]["exit_code"], 1)
                    emitted = events[0][1]["stdout"] + events[0][1]["stderr"]
                    self.assertIn("fatal error:", emitted)

    def test_rewrite_compiler_path_uses_toolchain_root_for_default_sdk(self):
        import os
        from unittest import mock

        from cli.preset import get_preset_compiler, rewrite_compiler_path

        custom = "/opt/custom/clang-glibc/bin/clang++"
        with mock.patch.dict(os.environ, {"URING_TOOLCHAIN_ROOT": "/opt/uring-sdks"}):
            resolved = get_preset_compiler("clang-glibc-release")
            gcc = get_preset_compiler("gcc-musl-release")
            untouched = rewrite_compiler_path(custom)
            home_gcc = rewrite_compiler_path(
                str(Path.home() / ".local/toolchain/gcc-glibc/bin/x86_64-unknown-linux-gnu-g++")
            )
        self.assertEqual(resolved, "/opt/uring-sdks/clang-glibc/bin/clang++")
        self.assertEqual(gcc, "/opt/uring-sdks/gcc-musl/bin/x86_64-unknown-linux-musl-g++")
        self.assertEqual(untouched, custom)
        self.assertEqual(home_gcc, "/opt/uring-sdks/gcc-glibc/bin/x86_64-unknown-linux-gnu-g++")
        env = {key: value for key, value in os.environ.items() if key != "URING_TOOLCHAIN_ROOT"}
        env["UARCH_TOOLCHAIN_ROOT"] = "/opt/uarch-sdks"
        with mock.patch.dict(os.environ, env, clear=True):
            uarch = get_preset_compiler("gcc-glibc-release")
            still_custom = rewrite_compiler_path(custom)
        self.assertEqual(uarch, "/opt/uarch-sdks/gcc-glibc/bin/x86_64-unknown-linux-gnu-g++")
        self.assertEqual(still_custom, custom)

    def test_supports_reflection_probe_is_a_consteval_function(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_tidy

        calls = []

        def fake_run(cmd, **kwargs):
            calls.append(list(cmd))
            source = Path(cmd[1]).read_text()
            self.assertIn("consteval bool Probe()", source)
            self.assertIn("static_assert(Probe())", source)
            self.assertNotIn("consteval auto", source)
            self.assertEqual(cmd[-3:], ["--", "-std=c++26", "-freflection"])
            return SimpleNamespace(returncode=0)

        with mock.patch.object(cmd_tidy.subprocess, "run", side_effect=fake_run):
            self.assertTrue(cmd_tidy.supports_reflection("/usr/bin/clang-tidy"))
        self.assertEqual(len(calls), 1)

    def test_supports_reflection_falls_back_to_latest_flag(self):
        from types import SimpleNamespace
        from unittest import mock

        from cli import cmd_tidy

        flags = []

        def fake_run(cmd, **kwargs):
            flags.append(cmd[-1])
            return SimpleNamespace(returncode=0 if cmd[-1] == "-freflection-latest" else 1)

        with mock.patch.object(cmd_tidy.subprocess, "run", side_effect=fake_run):
            self.assertTrue(cmd_tidy.supports_reflection("/usr/bin/clang-tidy"))
        self.assertEqual(flags, ["-freflection", "-freflection-latest"])

        def reject(cmd, **kwargs):
            flags.append(cmd[-1])
            return SimpleNamespace(returncode=1)

        flags.clear()
        with mock.patch.object(cmd_tidy.subprocess, "run", side_effect=reject):
            self.assertFalse(cmd_tidy.supports_reflection("/usr/bin/clang-tidy"))
        self.assertEqual(flags, ["-freflection", "-freflection-latest"])

    def test_run_tidy_fallback_suppresses_reflection_cascades(self):
        from cli import cmd_tidy

        crash = "clang-tidy: error: unable to find compilation database\n"
        failed, display = cmd_tidy.run_tidy(1, crash, fallback=True)
        self.assertTrue(failed)
        self.assertIn("compilation database", display)

        empty_failed, _empty_display = cmd_tidy.run_tidy(1, "", fallback=True)
        self.assertTrue(empty_failed)

        unknown_arg = "error: unknown argument: '-freflection' [clang-diagnostic-error]\n"
        failed, display = cmd_tidy.run_tidy(1, unknown_arg, fallback=True)
        self.assertTrue(failed)
        self.assertIn("-freflection", display)

        cross_file = (
            "a.cc:3:1: error: ^^int [clang-diagnostic-error]\n"
            "b.cc:900:1: error: use of undeclared identifier foo [clang-diagnostic-error]\n"
        )
        failed, display = cmd_tidy.run_tidy(1, cross_file, fallback=True)
        self.assertTrue(failed)
        self.assertIn("b.cc:900:1:", display)
        self.assertNotIn("a.cc", display)

        distant_same_file = (
            "a.cc:3:1: error: ^^int [clang-diagnostic-error]\n"
            "a.cc:900:1: error: use of undeclared identifier foo [clang-diagnostic-error]\n"
        )
        failed, display = cmd_tidy.run_tidy(1, distant_same_file, fallback=True)
        self.assertTrue(failed)
        self.assertIn("a.cc:900:1:", display)

        path_named_reflection = (
            "x/reflection.hpp:9:1: error: use of undeclared identifier foo [clang-diagnostic-error]\n"
        )
        failed, display = cmd_tidy.run_tidy(1, path_named_reflection, fallback=True)
        self.assertTrue(failed)

        uarch_only = (
            "b.cc:10:1: error: no viable conversion from uarch::BitWord to int [clang-diagnostic-error]\n"
        )
        failed, display = cmd_tidy.run_tidy(1, uarch_only, fallback=True)
        self.assertTrue(failed)

        cross_file_void = (
            "a.cc:3:1: error: ^^int [clang-diagnostic-error]\n"
            "b.cc:10:1: error: cannot initialize return object of type 'void' [clang-diagnostic-error]\n"
        )
        failed, display = cmd_tidy.run_tidy(1, cross_file_void, fallback=True)
        self.assertTrue(failed)
        self.assertIn("b.cc:10:1:", display)
        self.assertNotIn("a.cc", display)

        nearby_cascade = (
            "soa_columns.hpp:57:10: error: no member named 'meta' in namespace 'std' "
            "[clang-diagnostic-error]\n"
            "   57 |   using T = std::meta::info;\n"
            "soa_columns.hpp:61:93: error: use of undeclared identifier 'MemberType' "
            "[clang-diagnostic-error]\n"
        )
        failed, display = cmd_tidy.run_tidy(1, nearby_cascade, fallback=True)
        self.assertFalse(failed)
        self.assertEqual(display, "")

        single = "probe.cc:1:28: error: unsupported reflection operator ^^ [clang-diagnostic-error]\n"
        failed, display = cmd_tidy.run_tidy(1, single, fallback=True)
        self.assertFalse(failed)
        self.assertEqual(display, "")

        cascade = (
            "op_header.hpp:14:18: error: expected ']' [clang-diagnostic-error]\n"
            "   14 |   OpState state[[= uarch::bits::Field{0, 3}]];\n"
            "op_header.hpp:31:73: error: blocks support disabled - compile with -fblocks "
            "or pick a deployment target that supports them [clang-diagnostic-error]\n"
            "op_header.hpp:31:74: error: expected expression [clang-diagnostic-error]\n"
            "op_header.hpp:51:52: error: unused parameter 'kind' [clang-diagnostic-unused-parameter]\n"
            "reflect.hpp:28:70: error: no member named 'meta' in namespace 'std' [clang-diagnostic-error]\n"
            "   28 |   using Info = std::meta::info;\n"
        )
        failed, display = cmd_tidy.run_tidy(1, cascade, fallback=True)
        self.assertFalse(failed)
        self.assertEqual(display, "")

        plain = "a.cc:1:10: error: use of undeclared identifier 'foo' [clang-diagnostic-error]\n"
        failed, display = cmd_tidy.run_tidy(1, plain, fallback=True)
        self.assertTrue(failed)
        self.assertIn("undeclared identifier 'foo'", display)

        mixed = (
            "note uses ^^int\n"
            "a.cc:2:8: warning: invalid case style for struct 'bad_name' [readability-identifier-naming]\n"
        )
        failed, display = cmd_tidy.run_tidy(1, mixed, fallback=True)
        self.assertTrue(failed)
        self.assertIn("[readability-identifier-naming]", display)

        meta_header = "reflect.hpp:1:10: fatal error: 'meta' file not found\nnote: std::meta unavailable\n"
        failed, display = cmd_tidy.run_tidy(1, meta_header, fallback=True)
        self.assertFalse(failed)
        self.assertEqual(display, "")

        other_fatal = "a.cc:1:10: fatal error: 'missing.h' file not found\nnote: ^^int\n"
        failed, display = cmd_tidy.run_tidy(1, other_fatal, fallback=True)
        self.assertTrue(failed)
        self.assertIn("fatal error:", display)

        signal_failed, _signal_display = cmd_tidy.run_tidy(-9, "", fallback=True)
        self.assertTrue(signal_failed)

    def test_subdirectory_skips_toolchain_provision_and_lock_is_shared(self):
        toolchain = ROOT / "cmake" / "UringToolchain.cmake"
        text = toolchain.read_text()
        provision = text[text.index("function(_uring_provision_sdk") : text.index("\nfunction(uring_ensure_toolchain)")]
        uarch_lock = provision.index('.uarch-${sdk_name}.lock')
        uring_lock = provision.index('.uring-${sdk_name}.lock')
        download = provision.index("file(DOWNLOAD")
        self.assertLess(uarch_lock, uring_lock)
        self.assertLess(uring_lock, download)
        self.assertIn("string(RANDOM LENGTH 8 _stage_id)", provision)
        self.assertIn(".staging-${sdk_name}-${_stage_id}", provision)
        self.assertIn("${archive_path}.${_stage_id}.part", provision)
        rename_at = provision.index('file(RENAME "${archive_part}" "${archive_path}")')
        digest_at = provision.index('file(SHA256 "${archive_part}"')
        self.assertLess(digest_at, rename_at)
        ensure = text[text.index("function(uring_ensure_toolchain)") :]
        source_at = ensure.index("if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)")
        project_at = ensure.index("if(DEFINED PROJECT_IS_TOP_LEVEL AND NOT PROJECT_IS_TOP_LEVEL)")
        self.assertLess(source_at, project_at)
        self.assertEqual(ensure.count("if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)"), 1)
        self.assertIn("no SHA256 digest: ${digest_part}", provision)
        self.assertNotIn("no SHA256 digest: ${digest_path}", provision)
        self.assertNotIn("gnu-cross", text)
        self.assertNotIn("musl-cross", text)

        with tempfile.TemporaryDirectory(prefix="uring-spec-") as temporary:
            script = Path(temporary) / "spec.cmake"
            script.write_text(
                f'include("{toolchain}")\n'
                "foreach(name IN ITEMS clang-glibc clang-musl gcc-glibc gcc-musl)\n"
                "  _uring_toolchain_spec(\"${name}\" repo prefix compiler)\n"
                "  message(STATUS \"SPEC ${name} repo=${repo} prefix=${prefix} compiler=${compiler}\")\n"
                "endforeach()\n"
            )
            spec_run = subprocess.run(
                ["cmake", "-P", str(script)],
                check=False,
                capture_output=True,
                text=True,
            )
            spec_output = spec_run.stdout + spec_run.stderr
            self.assertEqual(spec_run.returncode, 0, spec_output)
            specs = {}
            for line in spec_output.splitlines():
                if "SPEC " not in line:
                    continue
                payload = line.split("SPEC ", 1)[1]
                name, rest = payload.split(" repo=", 1)
                repo, rest = rest.split(" prefix=", 1)
                prefix, compiler = rest.split(" compiler=", 1)
                specs[name] = (repo, prefix, compiler)
            self.assertEqual(
                specs,
                {
                    "clang-glibc": ("trunkszi/clang-cross", "x86_64-unknown-linux-gnu", "bin/clang++"),
                    "clang-musl": (
                        "trunkszi/clang-cross",
                        "x86_64-unknown-linux-musl",
                        "bin/x86_64-unknown-linux-musl-clang++",
                    ),
                    "gcc-glibc": (
                        "trunkszi/gcc-cross",
                        "x86_64-unknown-linux-gnu",
                        "bin/x86_64-unknown-linux-gnu-g++",
                    ),
                    "gcc-musl": (
                        "trunkszi/gcc-cross",
                        "x86_64-unknown-linux-musl",
                        "bin/x86_64-unknown-linux-musl-g++",
                    ),
                },
            )

        with tempfile.TemporaryDirectory(prefix="uring-subdir-") as temporary:
            parent = Path(temporary)
            sdk = parent / "missing-sdk"
            child = parent / "child"
            child.mkdir()
            (parent / "CMakeLists.txt").write_text(
                "cmake_minimum_required(VERSION 3.30)\n"
                "project(parent LANGUAGES NONE)\n"
                'set(URING_AUTO_DOWNLOAD_TOOLCHAIN OFF CACHE BOOL "" FORCE)\n'
                'set(URING_BUILD_TESTS ON CACHE BOOL "" FORCE)\n'
                'set(URING_TOOLCHAIN_NAME "clang-glibc" CACHE STRING "" FORCE)\n'
                f'set(URING_TOOLCHAIN_ROOT "{sdk}" CACHE PATH "" FORCE)\n'
                "add_subdirectory(child)\n"
            )
            (child / "CMakeLists.txt").write_text(
                f'include("{toolchain}")\n'
                "uring_ensure_toolchain()\n"
                'message(STATUS "SUBDIR_TOOLCHAIN_SKIPPED")\n'
            )
            configured = subprocess.run(
                ["cmake", "-S", str(parent), "-B", str(parent / "build")],
                check=False,
                capture_output=True,
                text=True,
            )
            combined = configured.stdout + configured.stderr
            self.assertEqual(configured.returncode, 0, combined)
            self.assertIn("SUBDIR_TOOLCHAIN_SKIPPED", combined)
            self.assertNotIn("Toolchain compiler is missing", combined)

            script = parent / "top.cmake"
            script.write_text(
                f'include("{toolchain}")\n'
                'set(URING_AUTO_DOWNLOAD_TOOLCHAIN OFF CACHE BOOL "" FORCE)\n'
                'set(URING_BUILD_TESTS ON CACHE BOOL "" FORCE)\n'
                'set(URING_TOOLCHAIN_NAME "clang-glibc" CACHE STRING "" FORCE)\n'
                f'set(URING_TOOLCHAIN_ROOT "{sdk}" CACHE PATH "" FORCE)\n'
                'set(CMAKE_CXX_COMPILER "" CACHE FILEPATH "" FORCE)\n'
                "uring_ensure_toolchain()\n"
            )
            top = subprocess.run(
                ["cmake", "-P", str(script)],
                cwd=parent,
                check=False,
                capture_output=True,
                text=True,
            )
            self.assertNotEqual(top.returncode, 0, top.stdout + top.stderr)
            self.assertIn("Toolchain compiler is missing", top.stdout + top.stderr)


if __name__ == "__main__":
    unittest.main()
