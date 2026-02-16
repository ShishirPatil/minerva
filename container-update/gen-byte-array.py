
import subprocess
from subprocess import PIPE
import sys

def main():
    if len(sys.argv) != 5:
        print("Supply arguments <input-elf> <output-file> <code-section-name> <data-section-name>")
        return;
    input_file = sys.argv[1]
    output_file = sys.argv[2]

    section_name_code = sys.argv[3]
    section_name_data = sys.argv[4]

    objdump = subprocess.run(["objdump", "-s", "-j", section_name_code, input_file], check=True, stdout=PIPE, stderr=PIPE).stdout.decode("utf-8")

    code_words = []
    for line in objdump.split('\n'):
        try:
            parts = line.split()[:-1]
            int(parts[0], 16)
            parts.pop(0)
            for i in range(4):
                int(parts[i], 16)
                if len(parts[i]) != 8:
                    continue
                code_words.append(parts[i])
        except:
            pass

    objdump = subprocess.run(["objdump", "-s", "-j", section_name_data, input_file], check=True, stdout=PIPE, stderr=PIPE).stdout.decode("utf-8")

    data_words = []
    for line in objdump.split('\n'):
        try:
            parts = line.split()[:-1]
            int(parts[0], 16)
            parts.pop(0)
            for i in range(4):
                int(parts[i], 16)
                if len(parts[i]) != 8:
                    print("Found", parts[i])
                    continue
                data_words.append(parts[i])
        except:
            pass

    with open(output_file, 'w') as f:
        f.write('unsigned char __ContainerCodeContents[] = {\n  ')
        for word in code_words[:-1]:
            if len(word) != 8: continue
            f.write('0x' + word[0:2] + ', ') 
            if len(word) > 2:
                f.write('0x' + word[2:4] + ', ') 
            if len(word) > 4:
                f.write('0x' + word[4:6] + ', ')
            if len(word) > 6:
                f.write('0x' + word[6:8] + ', ')
        f.write('0x' + code_words[-1][0:2] + ', ')
        if len(code_words[-1]) > 2:
            f.write('0x' + code_words[-1][2:4] + ', ')
        if len(code_words[-1]) > 4:
            f.write('0x' + code_words[-1][4:6] + ', ')
        if len(code_words[-1]) > 6:
            f.write('0x' + code_words[-1][6:8])
        f.write('\n};\n')
        f.write('unsigned int __ContainerCodeContentsLength = ' + str(len(code_words) * 4) + ';\n')
        f.write('unsigned char __ContainerDataContents[] = {\n  ')
        for word in data_words[:-1]:
            if len(word) != 8: continue
            f.write('0x' + word[0:2] + ', ')
            if len(word) > 2:
                f.write('0x' + word[2:4] + ', ') 
            if len(word) > 4:
                f.write('0x' + word[4:6] + ', ')
            if len(word) > 6:
                f.write('0x' + word[6:8] + ', ')
        f.write('0x' + data_words[-1][0:2] + ', ')
        if len(data_words[-1]) > 2:
            f.write('0x' + data_words[-1][2:4] + ', ')
        if len(data_words[-1]) > 4:
            f.write('0x' + data_words[-1][4:6] + ', ')
        if len(data_words[-1]) > 6:
            f.write('0x' + data_words[-1][6:8])
        f.write('\n};\n')
        f.write('unsigned int __ContainerDataContentsLength = ' + str(len(data_words) * 4) + ';\n')

    with open('container-update/checksum.h', 'w') as f:
        f.write('unsigned char __ContainerCodeContentsHash[] = {\n')
        subprocess.run(["gcc", "container-update/hash.c", "-o", "container-update/hash"], check=True, stdout=PIPE, stderr=PIPE).stdout.decode("utf-8")
        checksum = subprocess.run(["./container-update/hash"], check=True, stdout=PIPE, stderr=PIPE).stdout.decode("utf-8")
        for line in checksum.split('\n'):
            if (line == '='):
                f.write('};\n')
                f.write('unsigned char __ContainerDataContentsHash[] = {\n')
            else:
                f.write(line)
        f.write('};\n')



if __name__ == '__main__': main()


