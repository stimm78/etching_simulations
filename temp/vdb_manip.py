import os

tok = ["vdb_tool"]

def a(text):
	global tok
	tok.append(text)

vox=0.02

def make_plane(vox=vox, side=2.0, h1=-1.0, h2=0.0):
	h0 = h2-h1
	if h0 < side:
		z0 = 0.5*(h1+h2)
		a("-platonic faces=6 vox={:.3f} scale={:.3f} center=0.0,0.0,{:.1f}".format(
			vox, side, z0-0.5*(side - h0)))
		a("-platonic faces=6 vox={:.3f} scale={:.3f} center=0.0,0.0,{:.1f}".format(
			vox, side, z0+0.5*(side - h0)))
		a("-intersection vdb=0,1 keep=0")
	elif h0 > side:
		a("-platonic faces=6 vox={:.3f} scale={:.3f} center=0.0,0.0,{:.1f}".format(
			vox, side, h1+0.5*side))
		N = round(h0//side)
		for ii in range(N):
			a("-platonic faces=6 vox={:.3f} scale={:.3f} center=0.0,0.0,{:.1f}".format(
				vox, side, h2-(ii+0.5)*side))
			a("-union vdb=0,1 keep=0")
	else : # h0 == side
		a("-platonic faces=6 vox={:.3f} scale={:.3f} center=0.0,0.0,{:.1f}".format(
			vox, side, 0.5*(h1+h2)))
	return

"""
vox = 0.5
side = 20.0
h = 5.0
# a("-print")
a("-platonic faces=6 vox={:.3f} scale={:.3f} center=0.0,0.0,{:.1f}".format(
	vox, side, -0.5*side))
a("-platonic faces=6 vox={:.3f} scale={:.3f} center=0.0,0.0,{:.1f}".format(
	vox, side, 0.5*side - h))
a("-print")
# a("-platonic faces=6 dim={:d}".format(round(side/vox)))
# a("-resample vdb=0,0 keep=1 translate=0.0,0.0,{:.1f}".format(-0.5*(side-h)))
# a("-resample vdb=0 keep=1 scale=1.0 translate=0.0,0.0,{:.1f}".format(-0.5*(side-h)))
# a("-print")
a("-intersection vdb=0,1 keep=1")
# a("-prune vdb=0")
"""


# a("-debug")
h1h2_list = [
	(-1.0, 0.0),
	( 0.0, 4.0),
	( 4.0, 4.1),
]

for h1, h2 in h1h2_list:
	make_plane(h1=h1, h2=h2)
a("-print")

# a("-clip bbox=-10,-10,-5,10,10,8")
for ii, _ in enumerate(h1h2_list):
	a("-o test{:d}.vdb vdb={:d} keep=1".format(ii,ii))

command = " ".join(tok)
print(command)
os.system(command)