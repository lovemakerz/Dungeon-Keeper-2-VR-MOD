"""Run build tools with one environment entry per Windows variable name."""
import os
import subprocess
import sys

environment = {key.upper(): value for key, value in os.environ.items()}
environment['PATH'] = r'C:\Program Files\Git\usr\bin;C:\Program Files\Git\mingw64\bin;' + environment.get('PATH', '')
result = subprocess.run(sys.argv[1:], env=environment)
sys.exit(result.returncode)
