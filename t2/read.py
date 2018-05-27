#!/usr/bin/env python3

import re
import os

def le_arquivo(filename):
    f = open(filename, "r+")
    r_seq = r"(Tempo da busca sequencial:)[' ']*([\d]*)[' ']*[\w]*[' ']*([\d]*)"
    r_chunk = r"(Tempo da busca com threads:)[' ']*([\d]*)[' ']*[\w]*[' ']*([\d]*)[\w ]*([(][\w ]*)(chunk)"
    r_esparsa = r"(Tempo da busca com threads:)[' ']*([\d]*)[' ']*[\w]*[' ']*([\d]*)[\w ]*([(][\w ]*)(esparsa)"
    seq_count = chunk_count = esparsa_count = seq_tot = chunk_tot = esparsa_tot = 0
    for line in f:
        seq = re.search(r_seq, line)
        chunk = re.search(r_chunk, line)
        esparsa = re.search(r_esparsa, line)

        if seq:
            seq.group(3)
            seq_tot += int(seq.group(3))
            seq_count += 1

        if chunk:
            chunk.group(3)
            chunk_tot += int(chunk.group(3))
            chunk_count += 1
        
        if esparsa:
            esparsa.group(3)
            esparsa_tot += int(esparsa.group(3))
            esparsa_count += 1

    print(filename)
    print("Média sequencial: " + str(seq_tot / seq_count))
    print("Média c/ chunks: "+ str(chunk_tot / chunk_count))
    print("Média esparsa: "+ str(esparsa_tot / esparsa_count))
    print('\n')

def main():
    for file in os.listdir():
        filename = os.fsdecode(file)
        if filename.endswith(".txt"):
            le_arquivo(filename)
    

if __name__ == '__main__':
    main()
    