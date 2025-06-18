#!/bin/bash
# Test suite for cleanpath utility

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Test counter
TESTS=0
FAILURES=0

# Helper function to run a test
run_test() {
  local description="$1"
  local command="$2"
  local expected="$3"

  TESTS=$((TESTS + 1))

  # Run the command and capture output
  local actual
  actual=$(eval "$command" 2>&1)
  local exit_code=$?

  if [ "$actual" = "$expected" ]; then
    echo -e "${GREEN}✓${NC} $description"
    return 0
  else
    echo -e "${RED}✗${NC} $description"
    echo "  Command: $command"
    echo "  Expected: '$expected'"
    echo "  Actual: '$actual'"
    echo "  Exit code: $exit_code"
    FAILURES=$((FAILURES + 1))
    return 1
  fi
}

# Helper function to test exit codes
run_test_exit_code() {
  local description="$1"
  local command="$2"
  local expected_code="$3"

  TESTS=$((TESTS + 1))

  # Run the command and capture exit code
  eval "$command" >/dev/null 2>&1
  local actual_code=$?

  if [ "$actual_code" -eq "$expected_code" ]; then
    echo -e "${GREEN}✓${NC} $description (exit code $expected_code)"
    return 0
  else
    echo -e "${RED}✗${NC} $description"
    echo "  Command: $command"
    echo "  Expected exit code: $expected_code"
    echo "  Actual exit code: $actual_code"
    FAILURES=$((FAILURES + 1))
    return 1
  fi
}

# Test directory (will be set by setup_test_dirs)
TEST_DIR=""

# Create test directories
setup_test_dirs() {
  TEST_DIR=$(mktemp -d)
  if [ $? -ne 0 ]; then
    echo "Failed to create temporary test directory"
    exit 1
  fi

  mkdir -p "$TEST_DIR/dir1"
  mkdir -p "$TEST_DIR/dir2"
  mkdir -p "$TEST_DIR/dir with spaces"
  touch "$TEST_DIR/file1"
}

# Clean up test directories
cleanup_test_dirs() {
  if [ -n "$TEST_DIR" ] && [ -d "$TEST_DIR" ]; then
    rm -rf "$TEST_DIR"
  fi
}

echo "Running cleanpath tests..."
echo

# Setup test environment
setup_test_dirs

# Ensure cleanup on script exit or interruption
trap cleanup_test_dirs EXIT

# Basic functionality tests
echo "=== Basic Functionality ==="
run_test "Single valid path" \
  "./cleanpath /bin" \
  "/bin"

run_test "Multiple valid paths" \
  "./cleanpath /bin:/usr/bin" \
  "/bin:/usr/bin"

run_test "Remove duplicates" \
  "./cleanpath /bin:/usr/bin:/bin" \
  "/bin:/usr/bin"

run_test "Remove trailing slashes" \
  "./cleanpath /bin/:/usr/bin/" \
  "/bin:/usr/bin"

run_test "Handle root path" \
  "./cleanpath /" \
  "/"

run_test "Root path with trailing slash" \
  "./cleanpath //" \
  "/"

# Invalid path tests
echo
echo "=== Invalid Path Handling ==="
run_test "Skip non-existent paths" \
  "./cleanpath /bin:/nonexistent:/usr/bin" \
  "/bin:/usr/bin"

run_test "Skip relative paths" \
  "./cleanpath /bin:relative/path:/usr/bin" \
  "/bin:/usr/bin"

run_test "Skip files (not directories)" \
  "./cleanpath /bin:$TEST_DIR/file1:/usr/bin" \
  "/bin:/usr/bin"

run_test "Empty path components" \
  "./cleanpath /bin::/usr/bin:::/tmp" \
  "/bin:/usr/bin:/tmp"

run_test "Only empty components" \
  "./cleanpath :::::" \
  ""

# Separator tests
echo
echo "=== Separator Handling ==="
run_test "Custom separator -s" \
  "./cleanpath -s , /bin,/usr/bin" \
  "/bin,/usr/bin"

run_test "Custom separator -separator" \
  "./cleanpath -separator , /bin,/usr/bin" \
  "/bin,/usr/bin"

run_test "Custom separator --separator" \
  "./cleanpath --separator , /bin,/usr/bin" \
  "/bin,/usr/bin"

run_test "Semicolon separator" \
  "./cleanpath -s ';' '/bin;/usr/bin'" \
  "/bin;/usr/bin"

run_test "Space separator" \
  "./cleanpath -s ' ' '/bin /usr/bin'" \
  "/bin /usr/bin"

# Multiple arguments tests
echo
echo "=== Multiple Arguments ==="
run_test "Multiple path arguments" \
  "./cleanpath /bin:/usr/bin /tmp:/opt" \
  "/bin:/usr/bin:/tmp:/opt"

run_test "Multiple args with custom separator" \
  "./cleanpath -s , /bin,/usr/bin /tmp,/opt" \
  "/bin,/usr/bin,/tmp,/opt"

# Help tests
echo
echo "=== Help Flags ==="
run_test "Help flag -h shows usage" \
  "./cleanpath -h 2>&1 | head -1" \
  "Usage: ./cleanpath [OPTIONS] [PATH_LIST...]"

run_test "Help flag -help shows usage" \
  "./cleanpath -help 2>&1 | head -1" \
  "Usage: ./cleanpath [OPTIONS] [PATH_LIST...]"

run_test "Help flag --help shows usage" \
  "./cleanpath --help 2>&1 | head -1" \
  "Usage: ./cleanpath [OPTIONS] [PATH_LIST...]"

# Error handling tests
echo
echo "=== Error Handling ==="
run_test_exit_code "Unknown flag returns 255" \
  "./cleanpath -unknown" \
  255

run_test_exit_code "Separator without argument returns 252" \
  "./cleanpath -s" \
  252

run_test_exit_code "Multi-char separator returns 254" \
  "./cleanpath -s ab /bin" \
  254

run_test "Unknown flag error message" \
  "./cleanpath -unknown 2>&1 | grep -o 'unknown flag -unknown'" \
  "unknown flag -unknown"

run_test "Separator error message" \
  "./cleanpath -s 2>&1 | grep 'separator requires an argument' | head -1" \
  "-separator requires an argument"

# Edge cases
echo
echo "=== Edge Cases ==="
run_test "Very long path list" \
  "./cleanpath /bin:/usr/bin:/usr/local/bin:/sbin:/usr/sbin:/tmp:/bin:/usr/bin" \
  "/bin:/usr/bin:/usr/local/bin:/sbin:/usr/sbin:/tmp"

run_test "Path with spaces (if exists)" \
  "./cleanpath '$TEST_DIR/dir with spaces'" \
  "$TEST_DIR/dir with spaces"

run_test "Mixed valid and invalid paths" \
  "./cleanpath /nonexistent:/bin::/usr/bin:relative:/tmp:/bin/" \
  "/bin:/usr/bin:/tmp"

# Environment variable test (if PATH is set)
echo
echo "=== Environment Variable ==="
if [ -n "$PATH" ]; then
  run_test "No args uses PATH environment" \
    "./cleanpath >/dev/null && echo 'SUCCESS'" \
    "SUCCESS"
else
  echo "⚠ Skipping PATH environment test (PATH not set)"
fi

# Test with actual test directories
echo
echo "=== Test Directory Handling ==="
run_test "Test directories" \
  "./cleanpath $TEST_DIR/dir1:$TEST_DIR/dir2" \
  "$TEST_DIR/dir1:$TEST_DIR/dir2"

run_test "Test directory duplicates" \
  "./cleanpath $TEST_DIR/dir1:$TEST_DIR/dir1/" \
  "$TEST_DIR/dir1"

# Path length validation test
echo
echo "=== Path Length Validation ==="
# Create a very long path (over 4096 chars)
LONG_PATH="$TEST_DIR/"
for i in {1..1000}; do
  LONG_PATH="${LONG_PATH}verylongdirectoryname/"
done
run_test "Skip paths exceeding PATH_MAX" \
  "./cleanpath /bin:${LONG_PATH}:/usr/bin 2>&1 | grep -v 'Path too long'" \
  "/bin:/usr/bin"

# Summary
echo
echo "================================"
echo "Test Summary: $((TESTS - FAILURES))/$TESTS passed"

if [ $FAILURES -eq 0 ]; then
  echo -e "${GREEN}All tests passed!${NC}"
  exit 0
else
  echo -e "${RED}$FAILURES test(s) failed${NC}"
  exit 1
fi
