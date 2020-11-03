import os
import re
import sys
import subprocess

def which(program):
    import os
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None
    
class GreppableString(str):
    def chomp(self):
        return GreppableString(self.rstrip())

    def trim(self):
        return GreppableString(self.trim())

    def grep(self, pattern):
        return GreppableString(re.findall(pattern, self))

class Output:
    def __init__(self, exit_status, stdout, stderr, executable=None, utf8=True):
        self.exit_status = exit_status

        if utf8:
            try:
                if stdout: stdout = stdout.decode('utf8')
            except: 
                pass

            try:
                if stderr: stderr = stderr.decode('utf8')
            except:
                pass

        self.stdout = GreppableString(stdout)
        self.stderr = GreppableString(stderr)
        self.executable = executable
    
    @property
    def success(self):
        return self.exit_status == 0

    @property
    def failure(self):
        return self.exit_status != 0
        
class Executable:
    def __init__(self, filename):
        self.filename = filename
        self.verified = False

    @property
    def is_executable(self):
        if self.verified: return True
        if not self._is_executable(filename):
            return False
        self.verified = True
        return True

    def run(self, *args, stdin=None):
        if not self.is_executable:
            raise ValueError("Program %s is not executable!" % filename)

        p = subprocess.Popen([self.filename, *[str(a) for a in args]],
                             stdout=subprocess.PIPE,
                             stdin=subprocess.PIPE,
                             stderr=subprocess.STDOUT)

        stdout, stderr = p.communicate(input=stdin)
        return Output(p.returncode, stdout, stderr)

    def __call__(self, *args, **kwargs):
        return self.run(*args, **kwargs)

    def __str__(self):
        return self.stdout
        
    @staticmethod
    def _is_executable(filename):
        return which(filename) is not None
