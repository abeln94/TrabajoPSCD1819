
#include "Scoreboard.hpp"

#include <cassert>
#include <map>

//-----------------------------------------------------
//------------- CONSTRUCTOR / DESTRUCTOR --------------
//-----------------------------------------------------
Scoreboard::Scoreboard() {}
//-----------------------------------------------------
Scoreboard::~Scoreboard() {}
//-----------------------------------------------------
//------------------- FUNCTIONS -----------------------
//-----------------------------------------------------
void Scoreboard::PN(const Tupla& note) {
  // Post Note

  unique_lock<mutex> lck(mtxMonitor);

  // check if there are pending that want it
  auto p = pending.begin();
  while (p != pending.end()) {
    // for each pending

    if (_compare((*p).get().note, note)) {
      // the note can be assigned

      (*p).get().note = note;  // assign note

      // notify and remove
      (*p).get().condVar.notify_one();
      bool r = (*p).get().remove;
      p = pending.erase(p);  // remove now because we know where to find it

      if (r) {
        // note was removed, stop
        return;
      }
    } else {
      // the note can't be asigned, check next
      ++p;
    }
  }

  // note wasn't removed, add a copy to list
  notes.push_front(note);
}
//-----------------------------------------------------
Tupla Scoreboard::RN(const Tupla& note) {
  // Remove Note

  unique_lock<mutex> lck(mtxMonitor);
  return _rn(note, true, lck);
}
//-----------------------------------------------------
Tupla Scoreboard::readN(const Tupla& note) {
  // read Note

  unique_lock<mutex> lck(mtxMonitor);
  return _rn(note, false, lck);
}
//-----------------------------------------------------
//------------------INTERNAL---------------------------
//-----------------------------------------------------
Tupla Scoreboard::_rn(const Tupla& note, bool remove, unique_lock<mutex>& lck) {
  // Remove/read Note

  // check if there is a matching note
  auto p = notes.begin();
  while (p != notes.end()) {
    // for each note

    if (_compare(note, *p)) {
      // note can be assigned
      Tupla ret(*p);

      if (remove) {
        // note should be removed from list
        notes.erase(p);
      }
      return ret;
    }

    ++p;
  }

  // nothing found, wait for it
  Tupla ret(note);  // this will be modified with the matching note
  Scoreboard::PendingStruct pend(ret, remove);
  pending.push_back(pend);
  pend.condVar.wait(lck);
  // no need to remove from pending, already removed
  return ret;
}
//-----------------------------------------------------
bool Scoreboard::_compare(const Tupla& dst, const Tupla& src) {
  // compare patter tuple (dst) with tuple (src)

  if (dst.size() != src.size()) {
    // different sizes, doesn't match
    return false;
  }

  // check each element
  map<string, string> vars;  // to keep named variables
  for (int i = 0; i < src.size(); ++i) {
    // for each element of the notes

    if (dst[i] == "" || dst[i] == "?") {
      // unnamed variable, nothing to check

    } else if (dst[i][0] == '?') {
      // named variable, check

      string name = dst[i].substr(1, dst[i].size() - 1);

      // insert into map
      auto iterInsert = vars.insert(pair<string, string>(name, src[i]));
      if (!iterInsert.second && (iterInsert.first->second != src[i])) {
        // if not inserted and existing value different from new, doesn't match
        return false;
      }

    } else {
      // not a variable, check exact value

      if (dst[i] != src[i]) {
        // different value, doesn't match
        return false;
      }
    }
  }

  // notes match
  return true;
}
//-----------------------------------------------------
Scoreboard::PendingStruct::PendingStruct(Tupla& t, bool remove)
    : note(t), remove(remove) {};
