#include "Playlist.h"

// =======================
// Song implementation
// =======================

Song::Song(int id,
           string title,
           string artist,
           string album,
           int duration,
           int score,
           string url)
    : id(id),
      title(title),
      artist(artist),
      album(album),
      duration(duration),
      score(score),
      url(url),
      play_count(0)
{
}

string Song::toString() const {
    return "Song[id=" + to_string(id) + 
           ", title=\"" + title + 
           "\", artist=\"" + artist + 
           "\", album=\"" + album + 
           "\", duration=" + to_string(duration) + 
           ", score=" + to_string(score) + 
           ", url=\"" + url + 
           "\", play_count=" + to_string(play_count) + "]";
}

// =======================
// Playlist implementation
// =======================

Playlist::Playlist(string name)
    : name(name),
      size(0),
      currentIndex(-1)
#ifdef USE_THREADED_AVL
    , currentIt(songs.endIt()),
      hasCurrent(false)
#endif
{
}

Playlist::~Playlist() {
    clear();
}

SongKey Playlist::makeKey(Song* s) const {
    if (!s) return SongKey("", 0);
    return SongKey(s->getTitle(), s->getId());
}

void Playlist::resetPlayback() {
    currentIndex = -1;
#ifdef USE_THREADED_AVL
    hasCurrent = false;
    currentIt = songs.endIt();
#endif
}

int Playlist::getSize() const {
    return size;
}

bool Playlist::empty() const {
    return size == 0;
}

void Playlist::clear() {
    std::list<SongKey> keys = songs.ascendingList();
    for (const auto& k : keys) {
        Song** sPtr = songs.find(k);
        if (sPtr && *sPtr) {
            delete *sPtr; 
        }
    }
    songs.clear();
    size = 0;
    resetPlayback();
}

void Playlist::addSong(Song* s) {
   if (!s) return;
    SongKey key = makeKey(s);

    // Lưu lại bài hát đang phát hiện tại trước khi thêm mới
    Song* currentSong = nullptr;
#ifdef USE_THREADED_AVL
    if (hasCurrent) {
        currentSong = currentIt.value();
    }
#else
    if (currentIndex != -1) {
        currentSong = getSong(currentIndex);
    }
#endif

    // Thêm bài hát vào cây
    if (songs.insert(key, s)) {
        size++;
        
        if (currentSong) {
            SongKey currentKey = makeKey(currentSong);
            if (key < currentKey) {
                currentIndex++;
            }
        }
    }
}

void Playlist::removeSong(int index) {
   if (index < 0 || index >= size) return;
    
    Song* s = getSong(index);
    if (!s) return;

    bool currentRemoved = false;

#ifdef USE_THREADED_AVL
    if (hasCurrent) {
        if (index == currentIndex) {
            currentRemoved = true;
            if (size == 1) {
                currentIndex = -1;
                hasCurrent = false;
                currentRemoved = false;
            } else if (currentIndex == size - 1) {
                currentIndex = 0; // Vòng lại đầu nếu bài cuối bị xóa
            }
        } else if (index < currentIndex) {
            currentIndex--;
        }
    }
#else
    if (currentIndex != -1) {
        if (index == currentIndex) {
            currentRemoved = true;
            if (size == 1) {
                currentIndex = -1;
                currentRemoved = false;
            } else if (currentIndex == size - 1) {
                currentIndex = 0;
            }
        } else if (index < currentIndex) {
            currentIndex--;
        }
    }
#endif

    // Xóa bài hát khỏi cây AVL, có thể làm thay đổi cấu trúc node vật lý.
    songs.erase(makeKey(s)); 
    delete s; 
    s = nullptr; 
    size--;

#ifdef USE_THREADED_AVL
    //  Đồng bộ lại currentIt sau khi erase để tránh Iterator trỏ vào vùng nhớ đã free.
    if (hasCurrent) {
        currentIt = songs.beginIt();
        for (int i = 0; i < currentIndex; ++i) {
            ++currentIt;
        }
    }
#endif
}

Song* Playlist::getSong(int index) const {
    if (index < 0 || index >= size) return nullptr;

#ifdef USE_THREADED_AVL
    auto it = const_cast<Playlist*>(this)->songs.beginIt();
    for (int i = 0; i < index; ++i) ++it;
    return it.value();
#else
    std::list<SongKey> keys = songs.ascendingList();
    auto listIt = keys.begin();
    for (int i = 0; i < index; ++i) {
        ++listIt;
    }
    Song** sPtr = const_cast<Playlist*>(this)->songs.find(*listIt);
    return (sPtr) ? *sPtr : nullptr;
#endif
}

// Continuous playback
Song* Playlist::playNext() {
   if (size == 0) return nullptr;

#ifdef USE_THREADED_AVL
    if (!hasCurrent) {
        currentIt = songs.beginIt();
        currentIndex = 0;
        hasCurrent = true;
    } else {
        ++currentIt;
        currentIndex++;
        if (currentIt == songs.endIt()) {
            currentIt = songs.beginIt();
            currentIndex = 0;
        }
    }
    Song* s = currentIt.value();
    return s;
#else
    if (currentIndex == -1) {
        currentIndex = 0;
    } else {
        currentIndex = (currentIndex + 1) % size;
    }
    Song* s = getSong(currentIndex);
    return s;
#endif
}

Song* Playlist::playPrevious() {
    if (size == 0) return nullptr;

#ifdef USE_THREADED_AVL
    if (!hasCurrent) {
        currentIt = songs.rbeginIt();
        currentIndex = size - 1;
        hasCurrent = true;
    } else {
        if (currentIndex == 0) {
            currentIt = songs.rbeginIt();
            currentIndex = size - 1;
        } else {
            --currentIt;
            currentIndex--;
        }
    }
    Song* s = currentIt.value();
    return s;
#else
    if (currentIndex == -1) {
        currentIndex = size - 1;
    } else {
        currentIndex = (currentIndex - 1 + size) % size;
    }
    Song* s = getSong(currentIndex);
    return s;
#endif
}

int Playlist::getTotalScore() {
    int total = 0;
    std::list<SongKey> keys = songs.ascendingList();
    for (const auto& k : keys) {
        Song** sPtr = songs.find(k);
        if (sPtr && *sPtr) {
            total += (*sPtr)->getScore();
        }
    }
    return total;
}

bool Playlist::compareTo(const Playlist& p, int numSong) {
    int myScore = 0;
    int otherScore = 0;

    int limit1 = (size < numSong) ? size : numSong;
    std::list<SongKey> myKeys = songs.ascendingList();
    auto myIt = myKeys.begin();
    for (int i = 0; i < limit1; ++i) {
        Song** s = const_cast<Playlist*>(this)->songs.find(*myIt);
        if (s && *s) myScore += (*s)->getScore();
        ++myIt;
    }

    int limit2 = (p.getSize() < numSong) ? p.getSize() : numSong;
    std::list<SongKey> otherKeys = p.songs.ascendingList();
    auto otherIt = otherKeys.begin();
    for (int i = 0; i < limit2; ++i) {
        Song** s = const_cast<Playlist*>(&p)->songs.find(*otherIt);
        if (s && *s) otherScore += (*s)->getScore();
        ++otherIt;
    }

   return myScore >= otherScore;
}

// =======================
// Advanced playing modes
// =======================

void Playlist::playRandom(int index) {
    if (size == 0 || index < 0 || index >= size) return;
    
    currentIndex = index;

#ifdef USE_THREADED_AVL
    hasCurrent = true;
    currentIt = songs.beginIt();
    for (int i = 0; i < currentIndex; ++i) ++currentIt;
#else
    Song* s = getSong(currentIndex);
#endif
}

int Playlist::playApproximate(int step) {
   if (size == 0) return -1;

#ifdef USE_THREADED_AVL
    if (!hasCurrent) {
        currentIt = songs.beginIt();
        currentIndex = 0;
        hasCurrent = true;
    }
    
    // Tính toán index theo cơ chế cuốn vòng (Wrap-around / Circular)
    int targetIndex = (currentIndex + step) % size;
    if (targetIndex < 0) {
        targetIndex += size;
    }
    
    // Di chuyển an toàn iterator đến đúng vị trí target
    if (targetIndex > currentIndex) {
        for (int i = 0; i < targetIndex - currentIndex; ++i) ++currentIt;
    } else if (targetIndex < currentIndex) {
        for (int i = 0; i < currentIndex - targetIndex; ++i) --currentIt;
    }
    
    currentIndex = targetIndex;
    
    return currentIndex;
#else
    if (currentIndex == -1) {
        currentIndex = 0;
    }
    
    // Tính toán index theo cơ chế cuốn vòng (Wrap-around / Circular)
    currentIndex = (currentIndex + step) % size;
    if (currentIndex < 0) {
        currentIndex += size;
    }
    
    Song* s = getSong(currentIndex); 
    
    return currentIndex;
#endif
}