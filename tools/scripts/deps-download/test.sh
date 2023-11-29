#!/bin/bash

# Stop on error
set -e

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
echo "Script directory: ${script_dir}"

component_root_dir="$script_dir"/../../../

echo "Component root directory: ${component_root_dir}"