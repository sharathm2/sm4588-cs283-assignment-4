#!/usr/bin/env bats

@test "Example: check ls runs without errors" {
  run ./dsh <<EOF
ls
exit
EOF
  [ "$status" -eq 0 ]
}

@test "Check ls runs with additional arguments (ls -l)" {
  run ./dsh <<EOF
ls -l
exit
EOF
  [ "$status" -eq 0 ]
}

@test "Check cd command with valid absolute directory" {
  run ./dsh <<EOF
cd /
pwd
exit
EOF
  [ "$status" -eq 0 ]
  # Remove prompt output and extract any line that looks like an absolute directory
  pwd_line=$(echo "$output" | sed 's/^dsh2> //g' | egrep '^/' | head -n1)
  # The output from "pwd" should be just "/" when cd / was successful.
  [ "$pwd_line" = "/" ]
}

@test "Check cd command with no arguments does not change working directory" {
  # Get the current working directory of the test runner.
  current=$(pwd)
  run ./dsh <<EOF
pwd
cd
pwd
exit
EOF
  [ "$status" -eq 0 ]
  # Remove prompt strings and capture the two pwd outputs
  pwd_lines=$(echo "$output" | sed 's/^dsh2> //g' | egrep '^/')
  first_pwd=$(echo "$pwd_lines" | head -n1)
  second_pwd=$(echo "$pwd_lines" | tail -n1)
  [ "$first_pwd" = "$second_pwd" ]
  [ "$first_pwd" = "$current" ]
}

@test "Check cd command with relative directory (cd ..)" {
  # Get the current directory
  start_dir=$(pwd)
  run ./dsh <<EOF
pwd
cd ..
pwd
exit
EOF
  [ "$status" -eq 0 ]
  pwd_lines=$(echo "$output" | sed 's/^dsh2> //g' | egrep '^/')
  first=$(echo "$pwd_lines" | head -n1)
  second=$(echo "$pwd_lines" | tail -n1)
  # When not at root, cd .. should change the directory.
  [ "$first" != "$second" ]
  # Verify that the second directory is the parent of the first.
  expected_parent=$(dirname "$first")
  [ "$expected_parent" = "$second" ]
}

@test "Check cd command with invalid directory" {
  run ./dsh <<EOF
cd /nonexistent_directory
exit
EOF
  [ "$status" -ne 0 ]
}

@test "Check echo command with quoted string" {
  run ./dsh <<EOF
echo "hello world"
exit
EOF
  [ "$status" -eq 0 ]
  [[ "$output" == *"hello world"* ]]
}

@test "Check echo command with multiple spaces in quoted string" {
  run ./dsh <<EOF
echo "hello     world"
exit
EOF
  [ "$status" -eq 0 ]
  [[ "$output" == *"hello     world"* ]]
}

@test "Check exit command" {
  run ./dsh <<EOF
exit
EOF
  [ "$status" -eq 0 ]
}

@test "Check invalid command returns a nonzero status" {
  run ./dsh <<EOF
invalidcommand
exit
EOF
  [ "$status" -ne 0 ]
}

@test "Check that an empty command does not crash the shell" {
  run ./dsh <<EOF

exit
EOF
  [ "$status" -eq 0 ]
}