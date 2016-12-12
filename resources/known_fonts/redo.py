sans = open('sans.dat').readlines();
serif = open('serif.dat').readlines();

oldstyle = open('old_style.dat').readlines();
transitional = open('transitional.dat').readlines();
modern = open('modern.dat').readlines();
slab = open('slab.dat').readlines();

grotesque = open('grotesque.dat').readlines();
geometric = open('geometric.dat').readlines();
humanist = open('humanist.dat').readlines();

def contains(s, arr):
    for l in arr:
        if(l == s):
            if(s in sans):
                return True

def contains_sans(s):
    return contains(s, grotesque) or contains(s, geometric) or contains(s, humanist)

def contains_serif(s):
    return contains(s, oldstyle) or contains(s, transitional) or contains(s, modern) or contains(s, slab)

sans = [x for x in sans if not contains_sans(x)]
serif = [x for x in serif if not contains_serif(x)]
print(sans)
print(serif)
