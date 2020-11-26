from subprocess import Popen, PIPE

args = "../build/src/uno -c localhost:9091 -u bot"
p = Popen(args, shell=True, stdout=PIPE, stdin=PIPE)

while True:
    value = ' '
    value = bytes(value, 'UTF-8')
    p.stdin.write(value)
    p.stdin.flush()
    result = p.stdout.readline().decode('UTF-8') \
        .replace("\x1b[31m", "") \
        .replace("\x1b[32m", "") \
        .replace("\x1b[33m", "") \
        .replace("\x1b[34m", "") \
        .replace("\x1b[0m", "") \
        .replace("\n", "")

    print(result)
