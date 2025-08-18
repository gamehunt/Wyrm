#!/bin/bash

set -e

./build.sh
build/hatch test/1.dc
