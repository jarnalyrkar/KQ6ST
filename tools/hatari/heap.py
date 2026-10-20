import re,sys
v=dict((a,int(''.join(b.split()),16)) for a,b in re.findall(r'^0001E(A8E|B1E|B30|B40): ((?:[0-9a-f]{2} ){4})',open(sys.argv[1]).read(),re.M))
end,base,top,mx=v['A8E'],v['B1E'],v['B30'],v['B40']
print('heap size %d, used now %d, free now %d, max used %d (peak free %d)'%(end-base,top-base,end-top,mx-base,end-mx))
