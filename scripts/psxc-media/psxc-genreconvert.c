#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

char *genre_s[] = {
	"Blues", "Classic Rock", "Country", "Dance",
	"Disco", "Funk", "Grunge", "Hip-Hop",
	"Jazz", "Metal", "New Age", "Oldies",
	"Other", "Pop", "R&B", "Rap",
	"Reggae", "Rock", "Techno", "Industrial",
	"Alternative", "Ska", "Death Metal", "Pranks",
	"Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop",
	"Vocal", "Jazz+Funk", "Fusion", "Trance",
	"Classical", "Instrumental", "Acid", "House",
	"Game", "Sound Clip", "Gospel", "Noise",
	"Alt. Rock", "Bass", "Soul", "Punk",
	"Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
	"Ethnic", "Gothic", "Darkwave", "Techno-Industrial",
	"Electronic", "Pop-Folk", "Eurodance", "Dream",
	"Southern Rock", "Comedy", "Cult", "Gangsta Rap",
	"Top 40", "Christian Rap", "Pop Funk", "Jungle",
	"Native American", "Cabaret", "New Wave", "Psychedelic",
	"Rave", "Showtunes", "Trailer", "Lo-Fi",
	"Tribal", "Acid Punk", "Acid Jazz", "Polka",
	"Retro", "Musical", "Rock & Roll", "Hard Rock",
	"Folk", "Folk Rock", "National Folk", "Swing",
	"Fast-Fusion", "Bebob", "Latin", "Revival",
	"Celtic", "Bluegrass", "Avantgarde", "Gothic Rock",
	"Progressive Rock", "Psychedelic Rock", "Symphonic Rock", "Slow Rock",
	"Big Band", "Chorus", "Easy Listening", "Acoustic",
	"Humour", "Speech", "Chanson", "Opera",
	"Chamber Music", "Sonata", "Symphony", "Booty Bass",
	"Primus", "Porn Groove", "Satire", "Slow Jam",
	"Club", "Tango", "Samba", "Folklore",
	"Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle",
	"Duet", "Punk Rock", "Drum Solo", "A Cappella",
	"Euro-House", "Dance Hall", "Goa", "Drum & Bass",
	"Club-House", "Hardcore", "Terror", "Indie",
	"BritPop", "Negerpunk", "Polsk Punk", "Beat",
	"Christian Gangsta Rap", "Heavy Metal", "Black Metal", "Crossover",
	"Contemporary Christian", "Christian Rock", "Merengue", "Salsa",
	"Thrash Metal", "Anime", "JPop", "Synthpop",
	"Unknown"
};

int main(int argc, char **argv) {
	
	unsigned int genre = 0;
	if (argc < 2)
		return 0;
	genre = strtol(argv[1], (char **)NULL, 10);
	printf("%s\n", genre_s[genre < 149 ? genre : 13 ]);
	return 0;
}

