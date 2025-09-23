import pytest

def pytest_addoption(parser):
    parser.addoption(
        "--module-type",
        action="store",
        default="core",
        choices=["core", "discrete"],
        help="Module type to test (default: core)",
    )

# @pytest.fixture(autouse=True)
# def wait_prompt(request, dut):
#     dut.expect(">", timeout=10)
#     yield