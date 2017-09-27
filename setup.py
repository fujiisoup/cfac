import tempfile
from setuptools import setup, Command
from setuptools.command import install
import subprocess
import sys


class Make(Command):
    description = 'Custom build command'

    user_options = [ ('agree-cpc=', 'a', 'agree cpc license')]

    def initialize_options(self):
        self.agree_cpc = None

    def finalize_options(self):
        if self.agree_cpc is not None and self.agree_cpc != 'yes':
            raise ValueError("--agree-cpc should be yes. Given: {0:s}".format(
                             self.agree_cpc))

    def _call(self, commands):
        for command in commands:
            code = subprocess.call(command.split())
            if code != 0:
                raise ValueError('Install failed at ' + command)

    def autoconf(self):
        self._call(['autoconf -o configure ac-tools/configure.ac'])

    def configure(self):
        if self.agree_cpc is None:
            self._call(['./configure --prefix=$PWD/bin --with-cpc-modules'])
        else:
            command = './configure --prefix=$PWD/bin --with-cpc-modules'
            f = tempfile.TemporaryFile()
            p = subprocess.Popen(command.split(), stdin=subprocess.PIPE,
                                 stdout=f)
            p.stdin.write(b"yes\n")
            p.stdin.write(b"\n")
            f.close()

    def make(self):
        self._call(['make', 'make check', 'make install'])

    def clean(self):
        code = subprocess.call('make clean'.split())

    def run(self):
        self.autoconf()
        self.configure()
        self.clean()
        self.make()


setup(
    name='cfac',
    version='0.1',
    cmdclass={'make': Make}
)
