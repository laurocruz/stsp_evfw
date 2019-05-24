import urllib3 as urllib

http = urllib.PoolManager()
url = 'http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/index.html'
partial_url = url[:-10]

r = http.request('GET', url)
page = r.data.decode("utf-8")
page = page.split('\n')

i = 1
while not page[i - 1].startswith('<UL>'):
    i += 1

j = -1
while not page[j].startswith('</UL>'):
    j -= 1
page = page[i:j]

for l in page:
    if not l.startswith('<LI>'):
        continue
    file_name = l.split('\"')[1]
    file_url = partial_url + file_name
    print('Get {:s}'.format(file_name))
    content = http.request('GET', file_url).data.decode('utf-8')
    print('Write {:s}'.format(file_name))
    f = open(file_name, 'w')
    f.write(content)
    f.close()
    print('Success {:s}'.format(file_name))

print('\n'.join(page))
