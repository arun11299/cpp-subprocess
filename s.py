import subprocess

p = subprocess.Popen(["grep", "f"],
                     stdin = subprocess.PIPE,
                     stdout = subprocess.PIPE)
p.stdin.write('one\ntwo\four\n')
p.communicate()[0]
