import re

f = open("file.txt", "r")
r_seq = r"(Tempo da busca sequencial:)[' ']*([\d]*)[' ']*[\w]*[' ']*([\d]*)"
seq_count = 0
r_chunk = r"(Tempo da busca com threads:)[' ']*([\d]*)[' ']*[\w]*[' ']*([\d]*)[\w ]*([(][\w ]*)(chunk)"
chunk_count = 0
r_esparsa = r"(Tempo da busca com threads:)[' ']*([\d]*)[' ']*[\w]*[' ']*([\d]*)[\w ]*([(][\w ]*)(esparsa)"
esparsa_count = 0
seq_tot = 0
chunk_tot = 0
esparsa_tot = 0
for line in f:
    seq = re.search(r_seq, line)
    chunk = re.search(r_chunk, line)
    esparsa = re.search(r_esparsa, line)

    if seq:
        seq.group(3)
        seq_tot += int(seq.group(3))
        print(seq_tot)
        seq_count = seq_count + 1

    if chunk:
        chunk.group(3)
        chunk_tot += int(chunk.group(3))
        chunk_count+=1
    
    if esparsa:
        esparsa.group(3)
        esparsa_tot += int(esparsa.group(3))
        esparsa_count+=1
        
print("Média sequencial: " + str(seq_tot / seq_count))
print("Média c/ chunks: "+ str(chunk_tot / chunk_count))
print("Média esparsa: "+ str(esparsa_tot / esparsa_count))