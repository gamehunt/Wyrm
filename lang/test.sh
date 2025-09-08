#!/bin/bash

set -e

./build.sh
build/hatch test/hatch.dc
