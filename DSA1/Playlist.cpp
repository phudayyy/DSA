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
{
    this->id = id;
    this->title = title;
    this->artist = artist;
    this->album = album;
    this->duration = duration;
    this->score = score;
    this->url = url;
}

int Song::getID() const
{
    return this->id;
}

int Song::getDuration() const
{
    return this->duration;
}

int Song::getScore() const
{
    return this->score;
}

string Song::toString() const
{
    // TODO: Student implementation
    return title + "-" + artist;
}

// =======================
// Playlist implementation
// =======================

Playlist::Playlist(string name)
{
    // TODO: Student implementation
    this->name = name;
    size = 0;
    currentIndex = 0;
}

int Playlist::getSize() const
{
    // TODO: Student implementation
    return lstSong.size();
}

bool Playlist::empty() const
{
    // TODO: Student implementation
    return lstSong.empty();
}

void Playlist::clear()
{
    // TODO: Student implementation
    lstSong.clear();
    size = 0;
    currentIndex = 0;
}

void Playlist::addSong(Song* s)
{
    // TODO: Student implementation
     lstSong.add(s);
}

void Playlist::removeSong(int index)
{
    // TODO: Student implementation
      if (index < 0 || index >= lstSong.size()) {
        throw out_of_range("Index is invalid!");
    }

    lstSong.removeAt(index);

    if (index < currentIndex) {
        currentIndex--;          
    }
 
    if (lstSong.empty()) {
        currentIndex = 0;
    } else if (currentIndex >= lstSong.size()) {
        currentIndex = lstSong.size() - 1;
    }
}

Song* Playlist::getSong(int index) const
{
    // TODO: Student implementation
   if (index < 0 || index >= lstSong.size()){
     throw out_of_range("Index is invalid!");
   }
   return lstSong.get(index);
}

// =======================
// Playing control
// =======================

Song* Playlist::playNext()
{
    // TODO: Student implementation
    if (lstSong.empty()) {
        throw out_of_range("Index is invalid!");
    }

    if (lstSong.size() == 1) {
        currentIndex = 0;
        return lstSong.get(0);
    }

    if (currentIndex == lstSong.size() - 1) {
        currentIndex = 0;
    } else {
        currentIndex++;
    }

    return lstSong.get(currentIndex);
}

Song* Playlist::playPrevious()
{
    // TODO: Student implementation
    if (lstSong.empty()) {
        throw out_of_range("Index is invalid!");
    }

    if (lstSong.size() == 1) {
        currentIndex = 0;
        return lstSong.get(0);
    }

    if (currentIndex == 0) {
        currentIndex = lstSong.size() - 1;
    } else {
        currentIndex--;
    }

    return lstSong.get(currentIndex);
}

// =======================
// Score-related
// =======================

int Playlist::getTotalScore()
{
    // TODO: Student implementation
       int n = lstSong.size();
       int totalScore = 0;

       for (int i=0; i<n; i++) {
          int sum = 0;
          int minScore = lstSong.get(i)->getScore();

          for (int j=i; j<n; j++) {
            int score = lstSong.get(j)->getScore();
            sum = sum + score;

          if (score < minScore) {
            minScore = score;
             }

          totalScore = totalScore + minScore*sum;
          }

       }
    return totalScore;
}

bool Playlist::compareTo(Playlist p, int numSong)
{
    // TODO: Student implementation
    int nA = (int)lstSong.size();
    int nB = (int)p.lstSong.size();
    if (numSong <= 0 || nA < numSong || nB < numSong) {
        return false;
    }

    int countA = 0;
    double sumA = 0;
    
    for (int i=0; i<=nA - numSong; i++) {
    int maxScore = 0;

    for (int j=i; j<i + numSong; j++) {
        Song* currentSong = lstSong.get(j);
        int currentScore = currentSong->getScore();
        if (currentScore > maxScore){
            maxScore = currentScore;
        }
    }
    sumA += maxScore;
    countA++;
    }

    int countB = 0;
    double sumB = 0;
    for (int i=0; i<=nB - numSong; i++) {
    int maxScore = 0;

    for (int j=i; j<i + numSong; j++) {
        Song* currentSong = p.lstSong.get(j);
        int currentScore = currentSong->getScore();
        if (currentScore > maxScore){
            maxScore = currentScore;
        }
    }
    sumB += maxScore;
    countB++;
    }

   return (sumA/countA) >= (sumB/countB);

}

//hàm hỗ trợ copy constructor

Playlist::Playlist(const Playlist& other) {
    this->name = other.name;

    for (int i = 0; i < other.lstSong.size(); i++) {
        this->lstSong.add(other.lstSong.get(i));
    }
}


// =======================
// Advanced playing modes
// =======================

void Playlist::playRandom(int index)
{
    // TODO: Student implementation
    int n = lstSong.size();
    if (n == 0 || index < 0 || index >= n) return;

    bool* played = new bool[n];
    for (int i = 0; i < n; ++i) played[i] = false;
    played[index] = true;

    Song* currentSong = lstSong.get(index);
    int currentDuration = currentSong->getDuration();
    cout << currentSong->toString();

    int currentIdx = index;

    while (true) {
        int nextIdx = -1;
        int minDist = n + 1;
        for (int i = 0; i < n; ++i) {
            if (!played[i]) {
                Song* s = lstSong.get(i);
                if (s->getDuration() > currentDuration) {
                    int dist = abs(i - currentIdx);
                    if (dist < minDist) {
                        minDist = dist;
                        nextIdx = i;
                    } else if (dist == minDist) {
                        if (i < nextIdx) nextIdx = i;
                    }
                }
            }
        }
        if (nextIdx == -1) break;
        
        cout << "," << lstSong.get(nextIdx)->toString();
        played[nextIdx] = true;
        currentIdx = nextIdx;
        currentDuration = lstSong.get(nextIdx)->getDuration();
    }

    delete[] played;
}

int Playlist::playApproximate(int step)
{
    // TODO: Student implementation
    int n = lstSong.size();
    if (n <= 1) return 0;

    const int INF = 1000000000;
    int* dp = new int[n];

    dp[0] = 0;
    for (int i = 1; i < n; i++) {
        dp[i] = INF;
    }

    for (int i = 1; i < n; i++) {
        int dur_i = lstSong.get(i)->getDuration();

        for (int k = 1; k <= step + 1; k++) {
            if (i - k >= 0) {
                int dur_prev = lstSong.get(i - k)->getDuration();
                int diff = dur_i - dur_prev;
                if (diff < 0) diff = -diff;

                if (dp[i - k] + diff < dp[i]) {
                    dp[i] = dp[i - k] + diff;
                }
            }
        }
    }

    int result = dp[n - 1];
    delete[] dp;   

    return result;
}
