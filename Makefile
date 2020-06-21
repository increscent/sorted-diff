run: build
	./sorted-diff normal ./tests/test0/base.txt ./tests/test0/branch.txt ./tests/test0/additions.txt ./tests/test0/deletions.txt
	./sorted-merge ./tests/test0/base.txt ./tests/test0/branch.txt > ./tests/test0/output.txt

build: sorted-diff.c sorted-merge.c
	gcc -o sorted-diff sorted-diff.c
	gcc -o sorted-merge sorted-merge.c

test: build
	cd tests && ./test.sh
