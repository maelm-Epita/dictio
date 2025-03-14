#include "include/lettergroup.hpp"
#include "include/helpers.h"

LetterGroup* FindGroupWithIdentifier(std::string id, std::vector<LetterGroup> *groups){
    for (unsigned int i=0; i<groups->size(); i++){
        LetterGroup* grp = &(*groups)[i];
        if (grp->group_identifier == id){
            return grp;
        }
    }
    return nullptr;
}

std::string LetterGroup::LGStringFromLetterGroups(std::vector<LetterGroup> groups){
    std::string res;
    for (unsigned int i=0; i<groups.size(); i++){
        LetterGroup lg = groups[i];
        res += lg.group_identifier + ":";
        for (unsigned int j=0; j<lg.letters.size(); j++){
            Letter l = lg.letters[j];
            res += l.character;
            if (j!=lg.letters.size()-1){
                res += " ";
            }
        }
        if (i!=groups.size()-1){
            res += "\n";
        }
    }
    return res;
}

std::string LetterGroup::SPStringFromSyllablePattern(SyllablePattern ptn){
    std::string res;
    for (unsigned int j=0; j<ptn.size(); j++){
        LetterGroup* lg = ptn[j];
        res += lg->group_identifier;
        if (j!=ptn.size()-1){
            res += " ";
        }
    }
    return res;
}

std::string LetterGroup::SPStringFromSyllablePatterns(std::vector<SyllablePattern> patterns){
    std::string res;
    for (unsigned int i=0; i<patterns.size(); i++){
        SyllablePattern sp = patterns[i];
        res += SPStringFromSyllablePattern(sp);
        if (i!=patterns.size()-1){
            res += "\n";
        }
    }
    return res;
}


std::vector<LetterGroup> LetterGroup::LetterGroupsFromLGString(std::string string){
    std::vector<LetterGroup> res;
    std::vector<std::string> lgstrs = split(string, '\n');
    for (std::string lgrp : lgstrs){
        if (lgrp.empty()){
            continue;
        }
        LetterGroup grp;
        std::vector<std::string> lgtoks = split(lgrp, ':');
        std::string gid = lgtoks[0];
        grp.group_identifier = gid;
        std::string lstr = lgtoks[1];
        std::vector<std::string> lstrs = split(lstr, ' ');
        for (std::string lid : lstrs){
            Letter l;
            l.character = lid;
            grp.letters.push_back(l);
        }
        res.push_back(grp);
    }
    return res;
}

bool LetterGroup::SyllablePatternsFromSPString(std::string string, std::vector<LetterGroup> *grps, std::vector<SyllablePattern>* res){
    std::vector<std::string> ptrns = split(string, '\n');
    for (std::string ptrn : ptrns){
        if (ptrn.empty()){
            continue;
        }
        SyllablePattern pattern;
        std::vector<std::string> grpids = split(ptrn, ' ');
        for (std::string grpid : grpids ){
            LetterGroup* grp = FindGroupWithIdentifier(grpid, grps);
            if (grp == nullptr){
                return 1;
            }
            pattern.push_back(FindGroupWithIdentifier(grpid, grps));
        }
        res->push_back(pattern);
    }
    return 0;
}
