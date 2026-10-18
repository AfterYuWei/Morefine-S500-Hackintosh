#!/bin/sh
set -eu
PACK=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
exec /usr/bin/python3 "$PACK/check_current.py"
