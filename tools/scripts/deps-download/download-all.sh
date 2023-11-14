#!/bin/bash

# Stop on error
set -e

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
project_root_dir="$script_dir"/../../../

cd "${project_root_dir}"
mkdir -p submodules

# We pull the antlr jar to a known location inside the project hierarchy
# here so that it is in a known location for code generation
python3 "${script_dir}/download-dep.py" "${script_dir}/antlr4.json"

if [ -e "$project_root_dir/.git" ] ; then
  git submodule update --init --recursive
else
  python3 "${script_dir}/download-dep.py" "${script_dir}/abseil-cpp.json"
  python3 "${script_dir}/download-dep.py" "${script_dir}/date.json"
  python3 "${script_dir}/download-dep.py" "${script_dir}/json.json"
  python3 "${script_dir}/download-dep.py" "${script_dir}/simdjson.json"
  python3 "${script_dir}/download-dep.py" "${script_dir}/spdlog.json"
fi
