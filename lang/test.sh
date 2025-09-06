#!/bin/bash

set -e

./build.sh
build/hatch test/expr.dc
