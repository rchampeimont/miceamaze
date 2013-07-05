#include <string>
#include <stdio.h>
#include <stdlib.h>

int main() {
	for (int i=32; i<256; i++) {
		char s[1000];
		FILE *f = fopen("a.txt", "wb");
		fprintf(f, "%c", (char) i);
		fclose(f);
		sprintf(s, "convert -define png:color-type=6 -background transparent -fill white -font DejaVu-Sans-Book -pointsize 200 label:@a.txt ../project/rundir/data/images/chars/%03d.png", i);
		system(s);
		printf("%s\n", s);
	}
}

