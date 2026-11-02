#!/usr/bin/env python3
"""CI check of the version.

- version.txt is newer than the last release tag, or equal to the tag being built.
- Every reference to the version matches version.txt, CHANGELOG included.
- The documentation only needs the minor version (X.Y).
"""

import os
import re
import subprocess
import sys

# (file, regex whose group 1 is the version found, "full" for X.Y.Z or "minor" for X.Y)
REFERENCES = [
    ("README.md", r"badge/version-([\d.]+)-", "full"),
    ("components/openindus/idf_component.yml", r'^version:\s*"([^"]+)"', "full"),
    ("CHANGELOG.md", r"^# v{full}\s*$", "full"),
    ("docs/doxygen/Doxyfile", r'^PROJECT_NUMBER\s*=\s*"v(\d+\.\d+)', "minor"),
    ("docs/source/conf.py", r"'name': 'v({minor})\.\d+'", "minor"),
]


def parse(version):
    match = re.fullmatch(r"v?(\d+)\.(\d+)\.(\d+)(-[\w.]+)?", version.strip())
    if not match:
        sys.exit("not a version: %r" % version)
    return tuple(int(n) for n in match.groups()[:3])


def main():
    version = open("version.txt", encoding="utf-8").read().strip()
    numbers = parse(version)
    expected = {"full": "%d.%d.%d" % numbers, "minor": "%d.%d" % numbers[:2]}
    errors = []

    tag = os.environ.get("CI_COMMIT_TAG")
    tags = subprocess.run(["git", "tag", "--list", "v*"], capture_output=True, text=True, check=True).stdout.split()
    releases = [t for t in tags if re.fullmatch(r"v\d+\.\d+\.\d+", t) and t != tag]
    last = max(releases, key=parse) if releases else None
    if tag:
        if tag != version:
            errors.append("version.txt is %s but the tag is %s" % (version, tag))
    elif last and numbers <= parse(last):
        errors.append("version.txt (%s) is not newer than the last tag (%s)" % (version, last))

    for path, pattern, kind in REFERENCES:
        regex = pattern.replace("{full}", "(" + re.escape(expected["full"]) + ")").replace("{minor}", re.escape(expected["minor"]))
        match = re.search(regex, open(path, encoding="utf-8").read(), re.M)
        found = match.group(1) if match else None
        if found != expected[kind]:
            errors.append("%s: expected %s, found %s" % (path, expected[kind], found or "nothing"))

    for error in errors:
        print("ERROR:", error)
    print("version %s, last tag %s: %s" % (version, last, "KO" if errors else "OK"))
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())