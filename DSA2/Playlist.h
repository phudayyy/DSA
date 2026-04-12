#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "main.h"

#include <utility> // pair
#include <string>
// If implement ThreadedAVL, compile with -DUSE_THREADED_AVL
#ifdef USE_THREADED_AVL
#include "ThreadedAVL.h"
#else
#include "AVL.h"
#endif

// Key for ordering songs by alphabet (title), tie-break by id
using SongKey = std::pair<std::string, int>;

// =======================
// Class Song
// =======================
class Song
{
#ifdef TESTING
    friend class TestHelper;
#endif

private:
    int id;
    string title;
    string artist;
    string album;
    int duration;
    int score;
    string url;

    int play_count;

public:
    Song(int id,
         string title,
         string artist,
         string album,
         int duration,
         int score,
         string url);

    string toString() const;

    // TODO: Student can add more methods if needed
    int getId() const { return id; }
    string getTitle() const { return title; }
    int getScore() const { return score; }
    int getPlayCount() const { return play_count; }
    void increasePlayCount() { play_count++; }
};

// =======================
// Class Playlist
// =======================
class Playlist
{
#ifdef TESTING
    friend class TestHelper;
#endif

private:
    string name;
    int size;
    int currentIndex;

#ifdef USE_THREADED_AVL
    ThreadedAVL<SongKey, Song*> songs;
    ThreadedAVL<SongKey, Song*>::Iterator currentIt;
    bool hasCurrent;
#else
    AVL<SongKey, Song*> songs;
#endif

private:
    SongKey makeKey(Song* s) const;
    void resetPlayback();

public:
    Playlist(string name);
    ~Playlist(); // Bổ sung hàm hủy để giải phóng bộ nhớ cấp phát động

    Playlist(const Playlist&) = delete;
    Playlist& operator=(const Playlist&) = delete;

    int getSize() const;
    bool empty() const;
    void clear();

    void addSong(Song* s);
    void removeSong(int index);
    Song* getSong(int index) const;

    // Continuous playback
    Song* playNext();
    Song* playPrevious();

    int getTotalScore();
    bool compareTo(const Playlist& p, int numSong);

    // Advanced playing modes
    void playRandom(int index);
    int playApproximate(int step);
};

#endif /* PLAYLIST_H */