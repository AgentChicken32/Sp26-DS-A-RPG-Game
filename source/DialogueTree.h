#pragma once

#include "WorldMap.h"

#include <memory>
#include <string>
#include <vector>

struct DialogueNode;

struct DialogueChoice {
    std::string text;
    std::unique_ptr<DialogueNode> next;
};

struct DialogueNode {
    std::string speaker;
    std::string line;
    std::vector<DialogueChoice> choices;
};

struct DialogueTree {
    std::string npc_name;
    std::string encounter_text;
    std::unique_ptr<DialogueNode> root;
};

DialogueTree CreateRandomNpcDialogueTree(RegionId region, int random_roll);
