#include "DialogueTree.h"

#include <array>
#include <utility>

namespace {

struct NpcProfile {
    const char* name;
    const char* encounter_text;
    const char* opening_line;
    const char* region_rumor;
    const char* danger_warning;
    const char* request;
    const char* thanks;
    const char* travel_tip;
    const char* farewell;
};

std::unique_ptr<DialogueNode> MakeNode(const char* speaker, const char* line)
{
    auto node = std::make_unique<DialogueNode>();
    node->speaker = speaker;
    node->line = line;
    return node;
}

void AddChoice(DialogueNode& node,
               const char* text,
               std::unique_ptr<DialogueNode> next)
{
    node.choices.push_back(DialogueChoice{ text, std::move(next) });
}

DialogueTree BuildTree(const NpcProfile& profile)
{
    DialogueTree tree;
    tree.npc_name = profile.name;
    tree.encounter_text = profile.encounter_text;
    tree.root = MakeNode(profile.name, profile.opening_line);

    auto rumor = MakeNode(profile.name, profile.region_rumor);
    AddChoice(*rumor, "What should I watch for?",
              MakeNode(profile.name, profile.danger_warning));
    AddChoice(*rumor, "Any useful path from here?",
              MakeNode(profile.name, profile.travel_tip));

    auto request = MakeNode(profile.name, profile.request);
    AddChoice(*request, "I can spare a moment.",
              MakeNode(profile.name, profile.thanks));
    AddChoice(*request, "I need to keep moving.",
              MakeNode(profile.name, profile.farewell));

    AddChoice(*tree.root, "Ask about the region.", std::move(rumor));
    AddChoice(*tree.root, "Ask if they need help.", std::move(request));
    AddChoice(*tree.root, "Wish them safe travels.",
              MakeNode(profile.name, profile.farewell));

    return tree;
}

using RegionNpcProfiles = std::array<NpcProfile, 2>;

constexpr std::size_t RegionIndex(RegionId region)
{
    return static_cast<std::size_t>(region);
}

// Keep this table in RegionId enum order.
const std::array<RegionNpcProfiles, kRegionCount> kNpcProfiles = {{
    {{
            {
                "Runa the Ice Mason",
                "A mason chips frost from a blue-white archway and waves you closer.",
                "Keep your voice low. Sound carries through the ice like gossip through a tavern.",
                "The court was built over hot springs, but the old pipes froze shut when the storm years began.",
                "Do not trust clear ice. If you can see the stars through it, something below can see you.",
                "My chisel cracked. If you see black stone under the snow, mark it with your boot for me.",
                "Good. The next thaw will remember that kindness.",
                "The lee side of the northern wall blocks the worst wind and leads back toward Gelt.",
                "May your breath stay warm."
            },
            {
                "Hale Whitecloak",
                "A scout in a snow-crusted cloak studies the horizon with tired eyes.",
                "Another traveler? Either brave, lost, or paid very badly.",
                "There are lights under the ice tonight. They move against the wind.",
                "Rime wolves hunt in pairs. If you see one, the other is already behind you.",
                "I need someone to carry word south that the passes are narrowing.",
                "Then I will owe you a fire and a true map when we meet again.",
                "Follow the black marker poles. The pretty white ones are grave markers.",
                "Keep your shadow short out here."
            }
    }},
    {{
            {
                "Mira Netmender",
                "A netmender works copper hooks through a torn fishing net beside the docks.",
                "If you came for honest fish, you are late. If you came for rumors, sit.",
                "Ships from the north bring more sealed crates than cod these days.",
                "Smugglers use friendly smiles first and knives only after you turn around.",
                "A gull stole my needle case. If it drops near you, kick it back dockside.",
                "Ha. You have the look of someone gulls respect.",
                "The old coast road is safer at dawn, before the harbor crews start lying.",
                "Mind the tide and the men who pretend to own it."
            },
            {
                "Old Scrip",
                "An old trader counts coins with one hand and shells with the other.",
                "Every coin in Gelt has two stories. One is almost true.",
                "The ice traders are paying too much for lamp oil. That means a long dark is coming.",
                "Never follow a lantern alone. Real guides carry two.",
                "I need a witness while I seal this ledger. People behave when watched.",
                "Fine work. Numbers hate honest company.",
                "If the northern road smells like smoke, take the river road instead.",
                "Spend slowly. Run quickly."
            }
    }},
    {{
            {
                "Tamsin Pinewatch",
                "A ranger steps from the pines with a hand raised in greeting.",
                "Relax. If I meant to ambush you, we would be having a shorter talk.",
                "The trees have been bending east even when the wind comes west.",
                "Twilight belongs to the hungry things. Travel before noon when you can.",
                "I am tracking broken branches. Point out any you crossed on the trail.",
                "That gives me a line to follow. You may have saved someone's camp.",
                "Game trails curl toward the River, but count your steps or they curl back.",
                "Leave no bright cloth behind."
            },
            {
                "Orren Foxstep",
                "A trapper grins from beside a small, smokeless campfire.",
                "You look like you still believe maps. That is sweet.",
                "Something big dragged a pine clean out by the roots last night.",
                "If the forest goes silent, stop moving. Silence is a listener.",
                "I lost a snare line near the mossy stones. Untangle it if you pass by.",
                "Much obliged. I will name the least angry fox after you.",
                "The creek runs toward the River, but the dry bank is kinder to your boots.",
                "May your trail stay boring."
            }
    }},
    {{
            {
                "Jessa Reedhand",
                "A reed-cutter balances a bundle across one shoulder and nods.",
                "Careful where you step. The bank remembers every fool.",
                "The ferries are late because something keeps ringing bells under the water.",
                "Do not answer voices from the reeds unless you can see their feet.",
                "My bundle strap is slipping. Hold this end while I bind it.",
                "There. Now I can get home before the fog grows teeth.",
                "The east bend points toward the Channel, but the western shallows lead to Mudlands.",
                "Keep your socks dry if the gods allow it."
            },
            {
                "Captain Malo",
                "A barge captain leans on a pole and squints up at the clouds.",
                "Passenger? Guard? Omen? I have rates for two of those.",
                "Cargo has been arriving wet even when the barrels are sealed.",
                "River drakes like shiny buckles. Cover them unless you enjoy attention.",
                "Tell me if you see red flags downstream. I would rather not meet pirates twice.",
                "That warning will keep my crew breathing.",
                "Towpaths are faster than boats today. The current is in a mood.",
                "Fair water to you."
            }
    }},
    {{
            {
                "Brigg of the Bog Road",
                "A mud-spattered guide tests the ground ahead with a long pole.",
                "Step where I step unless you have grown fond of sinking.",
                "The road moved again last night. I swear it sulked when I cursed at it.",
                "Bubbles mean gas, still water means depth, and singing means run.",
                "I need fresh notches on the safe posts. Scratch one if you pass it.",
                "Good. Someone after you gets one less terrible surprise.",
                "High grass marks firmer ground toward the Rune Mountains.",
                "May the mud dislike your taste."
            },
            {
                "Nessa Mirelamp",
                "A woman with a hooded lantern lifts it just enough to show her face.",
                "The lamp is not for seeing. It is for being seen by the right things.",
                "Old bones surface after every hard rain. Some still wear keys.",
                "Never chase a light that matches your pace.",
                "My spare wick is damp. Shield the flame while I change it.",
                "There. You kept the dark polite for another hour.",
                "The western hummocks lead back to the River if you keep the moon on your left.",
                "Walk lightly, traveler."
            }
    }},
    {{
            {
                "Vell the Ferryman",
                "A ferryman steadies his skiff against the stone bank.",
                "Crossing is cheap. Coming back scared costs extra.",
                "The water has started flowing both directions at once near sunset.",
                "If you see your reflection smile first, step back from the edge.",
                "Tie that stern rope for me. My hands are numb from the current.",
                "Solid knot. Better than half the sailors who pay me in advice.",
                "Follow the bridge stones south to the Glade; north takes you back to the River.",
                "May your crossing be dull."
            },
            {
                "Sister Cale",
                "A road-sister chalks a small sign onto a bridge post.",
                "I mark safe roads. The unsafe ones mark themselves.",
                "Travelers have heard bells from beneath the channel stones.",
                "Bridge shadows sometimes lag behind people. Do not wait for yours.",
                "Hold my chalk while I fix the rain cover.",
                "Thank you. A dry sign saves more lives than a brave speech.",
                "The Glade path smells of cedar. The River path smells of wet rope.",
                "Go with clear eyes."
            }
    }},
    {{
            {
                "Elder Thorne",
                "An elder sits beside a standing stone warm with pale green light.",
                "The stones noticed you before I did. That is rarely meaningless.",
                "The Glade remembers every oath spoken under leaf and star.",
                "Do not carve your name here. Names are promises in this soil.",
                "Brush the moss from that lower rune. My knees have declared rebellion.",
                "There it is. The old mark breathes easier now.",
                "The eastern trail climbs into the Rune Mountains; the western one returns to the Channel.",
                "Listen when the leaves interrupt you."
            },
            {
                "Lio of the Stones",
                "A young watcher traces circles in the dirt around a rune stone.",
                "I am practicing prophecy. So far I predict dirt.",
                "The stones hum louder when storms gather over the eastern sea.",
                "Hollow deer are gentle until their antlers glow.",
                "Tell me if this circle looks round from where you stand.",
                "Excellent. I will call that a scholarly confirmation.",
                "Birdsong thins near the mountain trail. That is how you know the climb is close.",
                "May your omens be obvious."
            }
    }},
    {{
            {
                "Kara Stonecall",
                "A miner with chalk-white hands studies glowing script in the cliff.",
                "The mountain writes when it dreams. Today the handwriting is awful.",
                "New runes appeared above the east pass, all shaped like waves.",
                "Raiders scrape false runes into stone to lure travelers off cliffs.",
                "Hold this lantern while I copy the last line.",
                "Good light. The mountain did not blink once.",
                "The lower switchback leads to Patomic City if the fog stays thin.",
                "Step only on stone that sounds honest."
            },
            {
                "Bennit Chalkhand",
                "A cartographer kneels over a slate map covered in corrections.",
                "If this map changes one more time, I am charging it rent.",
                "The passes are shifting toward the sea, which is rude geography.",
                "Stonecut marauders leave polished pebbles where they plan ambushes.",
                "Mark that ridge for me while I hold the slate steady.",
                "Lovely. Now the map is only mostly lying.",
                "North drops toward Mudlands, south toward Patomic, east toward the sea road.",
                "May your path stay where you left it."
            }
    }},
    {{
            {
                "Keeper Elara",
                "A clockkeeper hurries through the square with brass tools at her belt.",
                "The city keeps ticking because too many stubborn people refuse to sleep.",
                "The clockwork ward is old, but it still answers courage better than fear.",
                "Thieves love crowds during a panic. Keep one hand on your purse.",
                "Hold this gear case while I reset the west chime.",
                "Thank you. If the bell sounds clean, pretend that was easy.",
                "The mountain road is hard, but it reaches the city faster than the coast in bad weather.",
                "The bells remember their friends."
            },
            {
                "Dockhand Sool",
                "A dockhand coils rope beside a stack of storm-battered crates.",
                "If you are looking for trouble, try Pier Three. If not, avoid Pier Three.",
                "The harbor lights flicker whenever the spiral clouds show their teeth.",
                "Do not help anyone carry a sealed crate unless you know who sealed it.",
                "Kick that loose wedge under the crate before it rolls into the bay.",
                "Nice footwork. The fish are denied a free meal.",
                "Ships can reach the Eastern Sea, but the Rune Mountains are steadier underfoot.",
                "Keep your boots above water."
            }
    }},
    {{
            {
                "Rase the Farrier",
                "A farrier checks a horse's shoe while grass ripples around the camp.",
                "Out here you can see trouble coming. The trick is admitting it is trouble.",
                "The herds have been running south before every storm bell.",
                "Grass hides holes, snakes, and proud warriors taking naps.",
                "Hold the reins while I set this last nail.",
                "Steady hands. The horse approves, which is rare praise.",
                "Follow the windbreak trees if you want the Southern Expanse.",
                "Ride straight, even on foot."
            },
            {
                "Mara Hoofsong",
                "A plains singer taps rhythm on a travel drum and smiles.",
                "Every road sounds different if you stop complaining long enough to hear it.",
                "The old songs skip verses near the eastern sea road now.",
                "If hoofbeats echo twice, one rider is not alive.",
                "Clap this beat back to me so I do not lose the verse.",
                "There. Now the road knows you have manners.",
                "Patomic lies west, the Southern Expanse south, and the sea waits east.",
                "May your song find a chorus."
            }
    }},
    {{
            {
                "Avi Coastreader",
                "A weathered guide shades their eyes against the bright southern road.",
                "Heat makes liars of distance. Drink before you feel wise.",
                "The shrine road opens only after the tide turns low and quiet.",
                "Mirages are harmless until they start calling your name.",
                "Help me weigh this trail ribbon with a stone.",
                "Good. The next traveler gets a friendlier warning.",
                "The Watchmaker's Shrine waits east by the old tide markers.",
                "Keep water close and pride closer."
            },
            {
                "Lenn Coastwatch",
                "A watchman polishes salt from a spyglass and nods toward the horizon.",
                "The sea has been practicing bad manners all week.",
                "Storm clouds circle offshore without moving inland. That means someone is steering.",
                "Coastline reavers hide sails behind low rocks until the last moment.",
                "Tell me if that smudge looks like a sail or a cloud.",
                "A sail, then. You may have bought us an hour.",
                "North leads back to the plains; east bends toward the shrine and the open sea.",
                "May the horizon stay honest."
            }
    }},
    {{
            {
                "Acolyte Verin",
                "An acolyte oils a small brass gear beside the shrine steps.",
                "Speak softly here. The old mechanisms are not asleep, only thinking.",
                "The shrine turns once for memory, once for warning, and once for debt.",
                "Never force a gear. The shrine dislikes impatience with sharp creativity.",
                "Pass me that clean rag before the salt gets into the teeth.",
                "Thank you. A quiet gear is a grateful gear.",
                "The Southern Expanse road is safest when the tide bells are silent.",
                "May your hour arrive gently."
            },
            {
                "Gear-Priest Oma",
                "A priest listens to the shrine wall with one ear pressed to the stone.",
                "It is counting. I do not yet know what it expects to reach.",
                "The Tideglass Sigil was made to turn storms aside, not destroy them.",
                "Broken sentinels still obey old commands if you speak like a thief.",
                "Stand there and tell me when the third tick becomes a chime.",
                "You heard it too. Good. I am not arguing with stone alone.",
                "Leave by the western stones unless the eastern lamps are lit.",
                "Time keep you kindly."
            }
    }},
    {{
            {
                "Selka Wavewise",
                "A sailor mends a sail with thread the color of sea foam.",
                "The sea is generous today. That always makes me suspicious.",
                "Currents have begun drawing circles around empty water.",
                "Never count waves past thirteen. Something may start counting back.",
                "Hold this seam tight while I stitch through the canvas.",
                "Strong grip. The sail might forgive both of us.",
                "Northwest returns to the Wilds; eastward routes split toward isles and cliffs.",
                "Fair wind, if fair wind still exists."
            },
            {
                "Torrin Saltcoat",
                "A salt-stained lookout lowers a brass spyglass.",
                "I saw your shape on the water before I saw you on the shore.",
                "Lights wink from Blinkering Isle even when the beacon is unmanned.",
                "Corsairs fly clean flags when they want you careless.",
                "Watch the eastern line and tell me if the light blinks twice.",
                "Twice. Then the island is calling again.",
                "The sea road touches Patomic, the plains, the mountains, and the storm isles.",
                "Keep one eye on shore."
            }
    }},
    {{
            {
                "Iona Stormglass",
                "A storm-reader catches rainwater in a cracked glass bowl.",
                "Do not stand too close. The bowl repeats what the sky is thinking.",
                "The spiral clouds are not weather. Weather forgets. This remembers.",
                "Lightning here dislikes metal, arrogance, and alphabetical order.",
                "Count the ripples while I cover the glass.",
                "Seven ripples. That is bad, but useful bad.",
                "The eastern chain can shelter you, but only if you reach it before dusk.",
                "May thunder choose another name."
            },
            {
                "Finn the Wrecklight",
                "A wreck scavenger shields a lantern with a patched sleeve.",
                "Every wreck here has a captain who thought one more mile was harmless.",
                "The reefs have moved into patterns that look almost written.",
                "If a dead bell rings underwater, leave the treasure to braver fools.",
                "Help me pull this rope free before the tide claims it.",
                "Fine pull. The tide looked offended.",
                "West returns to the open sea; east climbs toward the black cliffs.",
                "Keep your lantern mean and bright."
            }
    }},
    {{
            {
                "Pava Cliffguard",
                "A cliffguard braces a spear against the mountain wind.",
                "Talk fast or talk low. The wind steals anything in between.",
                "Storm rocs circle lower each day, as if the peaks are sinking.",
                "Loose stones fall before ambushes. Listen up, not down.",
                "Hold the marker flag while I retie the warning line.",
                "Good. That flag may keep a caravan from becoming scenery.",
                "The west pass drops to the sea; the south ridge points at Blinkering Isle.",
                "Keep your back off the cliff edge."
            },
            {
                "Eron Thunderhand",
                "A smith warms their hands near a coal box tucked between rocks.",
                "The mountain eats heat. I feed it scraps and insults.",
                "Iron rings strangely here, like it hears another hammer underground.",
                "Do not draw a blade during dry thunder unless you want a lesson.",
                "Pump the bellows twice while I close this brace.",
                "Perfect. The brace will hold unless the mountain takes that personally.",
                "The Storm Isles sit northwest, Blinkering Isle southeast, and the sea below both.",
                "May your metal stay quiet."
            }
    }},
    {{
            {
                "Luma Beaconkeep",
                "A beacon keeper trims a lamp wick inside the lighthouse door.",
                "If the light blinks wrong, ships argue with rocks. Rocks usually win.",
                "The beacon has started answering lights no one else can see.",
                "Never climb the tower if the stairs sound wet.",
                "Hold the lens cloth by the clean edge, please.",
                "Thank you. A clear lens is a small mercy with a long reach.",
                "Boats leave west for the Eastern Sea when the beacon burns steady.",
                "May you always see the shore first."
            },
            {
                "Noll Tidewatch",
                "A tidewatcher marks water levels on a post carved with old dates.",
                "The tide is late. I hate when the sea learns suspense.",
                "Some nights the island blinks before the lighthouse does.",
                "Beacon raiders wait for fog. If you smell pitch smoke, duck.",
                "Read that lower mark for me. My eyes are full of salt.",
                "Higher than yesterday. That is a problem, but now it has a number.",
                "The eastern cliffs are visible from the high path when fog allows it.",
                "Keep your lamp covered until you need it."
            }
    }},
    {{
            {
                "Velvet Toma",
                "A sharply dressed gambler shuffles cards without looking down.",
                "Welcome, traveler. The house smiles wider when heroes arrive.",
                "Everyone says the tables are fair. Everyone is part of the joke.",
                "Never bet against someone who asks your name before the first card.",
                "Cut the deck. I need a stranger's luck for this hand.",
                "Interesting cut. I will pretend not to be worried.",
                "Patomic City is one road away, but gamblers can make that road expensive.",
                "May your losses be educational."
            },
            {
                "Dealer Quin",
                "A dealer stacks chips into towers precise enough to offend gravity.",
                "No weapons on the felt, no crying on the carpet, no refunds from fate.",
                "A clockwork slot in the back room paid out thirteen times, then caught fire.",
                "If a table goes silent all at once, step away before the dice land.",
                "Straighten that chip stack for me before a manager sees it leaning.",
                "Clean work. You have the hands of someone who has touched danger.",
                "The city gates are west of the cashier cage. That is the only guaranteed exit.",
                "Leave while you still like arithmetic."
            }
    }}
}};
static_assert(kNpcProfiles.size() == kRegionCount);

const RegionNpcProfiles kFallbackProfiles = {{
        {
            "Wandering Pilgrim",
            "A traveler pauses beside the road and offers a cautious nod.",
            "Strange roads make quick friends of careful people.",
            "This place feels between stories, which is not always safe.",
            "Watch for tracks that begin without feet.",
            "Share the road a moment. It is easier to listen with company.",
            "Thank you. The road feels less empty now.",
            "Follow the landmarks you trust and doubt the ones that move.",
            "Safe steps to you."
        },
        {
            "Roadside Merchant",
            "A merchant adjusts a pack full of clinking tins.",
            "I sell needles, salt, and unsolicited opinions.",
            "The roads have been rearranging themselves around nervous travelers.",
            "A bargain offered too quickly is usually bait.",
            "Tell me if this strap is about to snap.",
            "Good eye. That saved me a very loud afternoon.",
            "Main roads are slower, side roads are hungrier.",
            "Spend wisely, breathe often."
        }
}};

const RegionNpcProfiles& ProfilesForRegion(RegionId region)
{
    const std::size_t index = RegionIndex(region);
    if (index < kNpcProfiles.size()) {
        return kNpcProfiles[index];
    }
    return kFallbackProfiles;
}
} // namespace

DialogueTree CreateRandomNpcDialogueTree(RegionId region, int random_roll)
{
    const auto& profiles = ProfilesForRegion(region);
    if (random_roll < 0) {
        random_roll = 0;
    }

    const std::size_t index =
        static_cast<std::size_t>(random_roll) % profiles.size();
    return BuildTree(profiles[index]);
}
