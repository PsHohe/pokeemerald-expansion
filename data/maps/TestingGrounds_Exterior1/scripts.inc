TestingGrounds_Exterior1_MapScripts::
# 5 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	map_script MAP_SCRIPT_ON_TRANSITION, TestingGrounds_Exterior1_MapScripts_MAP_SCRIPT_ON_TRANSITION
	.byte 0

TestingGrounds_Exterior1_MapScripts_MAP_SCRIPT_ON_TRANSITION:
# 6 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	setflag FLAG_TEMP_2
	return


TestingGrounds_Exterior1_EventScript_GiftNpc::
# 15 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	lock
# 16 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	faceplayer
# 17 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	goto_if_set FLAG_UNUSED_0x020, TestingGrounds_Exterior1_EventScript_GiftNpc_2
# 22 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferGymProgress
# 23 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_GiftIntro
# 24 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_GiveGymStarters
# 25 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, 0
	goto_if_eq TestingGrounds_Exterior1_EventScript_GiftNpc_5
# 30 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	setflag FLAG_UNUSED_0x020
# 31 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	setflag FLAG_SYS_POKEMON_GET
# 32 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	playfanfare MUS_OBTAIN_ITEM
# 33 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	message TestingGrounds_Exterior1_Text_GiftReceived
# 34 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitfanfare
# 35 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitmessage
# 36 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_AssignSignatureTM
# 37 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	setflag FLAG_TM_GROUP_STARTER
# 38 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	playfanfare MUS_OBTAIN_TMHM
# 39 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	message TestingGrounds_Exterior1_Text_ReceivedSignatureTM
# 40 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitfanfare
# 41 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitmessage
# 42 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_ExplainSignatureTM
# 43 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferRankInfo
# 44 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_GiftExplainRules
# 45 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_GiftNpc_2:
# 18 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_GiftPostGift
# 19 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_GiftNpc_5:
# 26 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_GiftNoRoom
# 27 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end


TestingGrounds_Exterior1_EventScript_GymAttendant::
# 56 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	lock
# 57 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	faceplayer
# 58 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_OpenGymPrompt, MSGBOX_YESNO
# 59 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, NO
	goto_if_eq TestingGrounds_Exterior1_EventScript_GymAttendant_2
# 64 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	message TestingGrounds_Exterior1_Text_ChooseTeamSource
# 65 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitmessage
# 66 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	dynmultichoice 0, 0, FALSE, 7, 0, DYN_MULTICHOICE_CB_NONE, TestingGrounds_Exterior1_Text_SourceParty, TestingGrounds_Exterior1_Text_Team1, TestingGrounds_Exterior1_Text_Team2, TestingGrounds_Exterior1_Text_Team3, TestingGrounds_Exterior1_Text_Team4, TestingGrounds_Exterior1_Text_Team5, TestingGrounds_Exterior1_Text_SourceCancel
# 74 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, MULTI_B_PRESSED
	goto_if_eq TestingGrounds_Exterior1_EventScript_GymAttendant_5
# 74 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, 6
	goto_if_eq TestingGrounds_Exterior1_EventScript_GymAttendant_5
# 79 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, 0
	goto_if_eq TestingGrounds_Exterior1_EventScript_GymAttendant_10
# 82 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	copyvar VAR_0x8004, VAR_RESULT
# 83 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	subvar VAR_0x8004, 1
# 84 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymTeams_LoadTeamIntoParty
# 85 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, 0
	goto_if_eq TestingGrounds_Exterior1_EventScript_GymAttendant_16
TestingGrounds_Exterior1_EventScript_GymAttendant_9:
# 91 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferRankInfo
# 92 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_PickDefenders
# 93 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_IsDoubles
# 94 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, TRUE
	goto_if_eq TestingGrounds_Exterior1_EventScript_GymAttendant_14
TestingGrounds_Exterior1_EventScript_GymAttendant_13:
# 97 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	fadescreen FADE_TO_BLACK
# 98 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_ChooseGymParty
# 99 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, 0
	goto_if_eq TestingGrounds_Exterior1_EventScript_GymAttendant_19
# 105 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special ReducePlayerPartyToSelectedMons
# 106 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_Begin
# 107 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	switch VAR_RESULT
# 108 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case GYM_TEAM_WRONG_SIZE, TestingGrounds_Exterior1_EventScript_GymAttendant_23
# 114 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case GYM_TEAM_OVERLEVELED, TestingGrounds_Exterior1_EventScript_GymAttendant_24
# 120 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case GYM_TEAM_WRONG_TYPE, TestingGrounds_Exterior1_EventScript_GymAttendant_25
# 127 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_GymIsOpen
# 128 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_GetStreakTier
# 129 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	switch VAR_RESULT
# 130 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case 0, TestingGrounds_Exterior1_EventScript_GymAttendant_28
# 131 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case 1, TestingGrounds_Exterior1_EventScript_GymAttendant_29
# 132 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case 2, TestingGrounds_Exterior1_EventScript_GymAttendant_30
# 133 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case 3, TestingGrounds_Exterior1_EventScript_GymAttendant_31
TestingGrounds_Exterior1_EventScript_GymAttendant_26:
# 135 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	closemessage
# 136 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	setvar VAR_TEMP_3, 0
TestingGrounds_Exterior1_EventScript_GymAttendant_33:
# 137 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_TEMP_3, GYM_NUM_CHALLENGES_PER_WAVE
	goto_if_lt TestingGrounds_Exterior1_EventScript_GymAttendant_34
# 185 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_End
# 186 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special LoadPlayerParty
# 187 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special HealPlayerParty
# 188 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_GymDayComplete
# 189 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_IsRankUpUnlocked
# 190 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, TRUE
	goto_if_eq TestingGrounds_Exterior1_EventScript_GymAttendant_37
TestingGrounds_Exterior1_EventScript_GymAttendant_36:
# 193 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_GymAttendant_2:
# 60 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_ComeBackAnytime
# 61 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_GymAttendant_5:
# 75 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_ComeBackAnytime
# 76 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_GymAttendant_10:
# 80 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special SavePlayerParty
	goto TestingGrounds_Exterior1_EventScript_GymAttendant_9

TestingGrounds_Exterior1_EventScript_GymAttendant_14:
# 95 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_DoublesReminder
	goto TestingGrounds_Exterior1_EventScript_GymAttendant_13

TestingGrounds_Exterior1_EventScript_GymAttendant_16:
# 86 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_TeamUnavailable
# 87 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_GymAttendant_19:
# 100 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special LoadPlayerParty
# 101 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_ComeBackAnytime
# 102 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_GymAttendant_23:
# 109 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special LoadPlayerParty
# 110 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferRankInfo
# 111 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_TeamWrongSize
# 112 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_GymAttendant_24:
# 115 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special LoadPlayerParty
# 116 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferRankInfo
# 117 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_TeamOverleveled
# 118 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_GymAttendant_25:
# 121 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special LoadPlayerParty
# 122 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferGymProgress
# 123 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_TeamWrongType
# 124 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_GymAttendant_28:
# 130 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_StreakPhraseCalm
	goto TestingGrounds_Exterior1_EventScript_GymAttendant_26

TestingGrounds_Exterior1_EventScript_GymAttendant_29:
# 131 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_StreakPhraseStirring
	goto TestingGrounds_Exterior1_EventScript_GymAttendant_26

TestingGrounds_Exterior1_EventScript_GymAttendant_30:
# 132 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_StreakPhraseSpreading
	goto TestingGrounds_Exterior1_EventScript_GymAttendant_26

TestingGrounds_Exterior1_EventScript_GymAttendant_31:
# 133 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_StreakPhrasePeak
	goto TestingGrounds_Exterior1_EventScript_GymAttendant_26

TestingGrounds_Exterior1_EventScript_GymAttendant_34:
# 138 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_TEMP_3, 0
	goto_if_gt TestingGrounds_Exterior1_EventScript_GymAttendant_40
TestingGrounds_Exterior1_EventScript_GymAttendant_39:
# 142 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_PickChallenger
# 143 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	addobject LOCALID_TESTING_GYM_CHALLENGER
# 144 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	applymovement LOCALID_TESTING_GYM_CHALLENGER, TestingGrounds_Exterior1_EventScript_GymAttendant_Movement_0
# 145 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitmovement 0
# 146 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_0x8006, GYM_TIER_STRONG
	goto_if_eq TestingGrounds_Exterior1_EventScript_GymAttendant_43
TestingGrounds_Exterior1_EventScript_GymAttendant_42:
# 151 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferChallengerIntro
# 152 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox gStringVar4
# 153 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special HealPlayerParty
# 154 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_StartBattle
# 155 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	specialvar VAR_RESULT, GetBattleOutcome
# 156 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, B_OUTCOME_WON
	goto_if_ne TestingGrounds_Exterior1_EventScript_GymAttendant_46
# 170 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_ChallengerDefeated
# 171 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_AwardVictoryPoints
# 172 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	playfanfare MUS_OBTAIN_B_POINTS
# 173 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	message TestingGrounds_Exterior1_Text_EarnedGymPoints
# 174 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitfanfare
# 175 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitmessage
# 176 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, TRUE
	goto_if_eq TestingGrounds_Exterior1_EventScript_GymAttendant_49
TestingGrounds_Exterior1_EventScript_GymAttendant_48:
# 179 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	closemessage
# 180 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	applymovement LOCALID_TESTING_GYM_CHALLENGER, TestingGrounds_Exterior1_EventScript_GymAttendant_Movement_1
# 181 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitmovement 0
# 182 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	removeobject LOCALID_TESTING_GYM_CHALLENGER
# 183 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	addvar VAR_TEMP_3, 1
	goto TestingGrounds_Exterior1_EventScript_GymAttendant_33

TestingGrounds_Exterior1_EventScript_GymAttendant_37:
# 191 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_ExamAvailable
	goto TestingGrounds_Exterior1_EventScript_GymAttendant_36

TestingGrounds_Exterior1_EventScript_GymAttendant_40:
# 139 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_NextChallengerArrived
# 140 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	closemessage
	goto TestingGrounds_Exterior1_EventScript_GymAttendant_39

TestingGrounds_Exterior1_EventScript_GymAttendant_43:
# 147 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_StrongChallengerWarning
# 148 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	closemessage
	goto TestingGrounds_Exterior1_EventScript_GymAttendant_42

TestingGrounds_Exterior1_EventScript_GymAttendant_46:
# 157 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_RecordLoss
# 158 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_LeaderLost
# 159 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	closemessage
# 160 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	applymovement LOCALID_TESTING_GYM_CHALLENGER, TestingGrounds_Exterior1_EventScript_GymAttendant_Movement_1
# 161 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitmovement 0
# 162 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	removeobject LOCALID_TESTING_GYM_CHALLENGER
# 163 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_End
# 164 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special LoadPlayerParty
# 165 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special HealPlayerParty
# 166 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_GymClosedAfterLoss
# 167 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_GymAttendant_49:
# 177 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_BarFilled
	goto TestingGrounds_Exterior1_EventScript_GymAttendant_48


TestingGrounds_Exterior1_EventScript_TeamKeeper::
# 202 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	lock
# 203 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	faceplayer
# 204 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_TeamKeeperIntro
TestingGrounds_Exterior1_EventScript_TeamKeeper_2:
# 206 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	message TestingGrounds_Exterior1_Text_ChooseTeam
# 207 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitmessage
# 208 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	dynmultichoice 0, 0, FALSE, 6, 0, DYN_MULTICHOICE_CB_NONE, TestingGrounds_Exterior1_Text_Team1, TestingGrounds_Exterior1_Text_Team2, TestingGrounds_Exterior1_Text_Team3, TestingGrounds_Exterior1_Text_Team4, TestingGrounds_Exterior1_Text_Team5, TestingGrounds_Exterior1_Text_TeamMenuExit
# 215 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, MULTI_B_PRESSED
	goto_if_eq TestingGrounds_Exterior1_EventScript_TeamKeeper_5
# 215 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, GYM_NUM_SAVED_TEAMS
	goto_if_eq TestingGrounds_Exterior1_EventScript_TeamKeeper_5
# 218 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	copyvar VAR_0x8004, VAR_RESULT
# 219 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	copyvar VAR_TEMP_1, VAR_RESULT
# 220 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	addvar VAR_TEMP_1, 1
# 221 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	buffernumberstring STR_VAR_2, VAR_TEMP_1
# 222 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymTeams_BufferTeam
# 223 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, 0
	goto_if_eq TestingGrounds_Exterior1_EventScript_TeamKeeper_10
# 226 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_TeamMembers
TestingGrounds_Exterior1_EventScript_TeamKeeper_9:
# 228 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	closemessage
# 229 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	dynmultichoice 0, 0, FALSE, 3, 0, DYN_MULTICHOICE_CB_NONE, TestingGrounds_Exterior1_Text_TeamActionRegister, TestingGrounds_Exterior1_Text_TeamActionClear, TestingGrounds_Exterior1_Text_TeamActionBack
# 233 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	switch VAR_RESULT
# 234 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case 0, TestingGrounds_Exterior1_EventScript_TeamKeeper_14
# 237 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case 1, TestingGrounds_Exterior1_EventScript_TeamKeeper_15
	goto TestingGrounds_Exterior1_EventScript_TeamKeeper_2

TestingGrounds_Exterior1_EventScript_TeamKeeper_1:
# 242 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_TeamKeeperBye
# 243 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_TeamKeeper_5:
	goto TestingGrounds_Exterior1_EventScript_TeamKeeper_1

TestingGrounds_Exterior1_EventScript_TeamKeeper_10:
# 224 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_TeamEmpty
	goto TestingGrounds_Exterior1_EventScript_TeamKeeper_9

TestingGrounds_Exterior1_EventScript_TeamKeeper_14:
# 235 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymTeams_RegisterParty
# 236 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_TeamRegistered
	goto TestingGrounds_Exterior1_EventScript_TeamKeeper_2

TestingGrounds_Exterior1_EventScript_TeamKeeper_15:
# 238 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymTeams_ClearTeam
# 239 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_TeamCleared
	goto TestingGrounds_Exterior1_EventScript_TeamKeeper_2


TestingGrounds_Exterior1_EventScript_RankExaminer::
# 252 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	lock
# 253 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	faceplayer
# 254 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferRankInfo
# 255 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_RankInfo
# 256 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_IsDoubles
# 257 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, TRUE
	goto_if_eq TestingGrounds_Exterior1_EventScript_RankExaminer_2
TestingGrounds_Exterior1_EventScript_RankExaminer_1:
# 260 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferSignatureMove
# 261 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_SignatureMoveInfo
# 262 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_GYM_RANK, 3
	goto_if_ge TestingGrounds_Exterior1_EventScript_RankExaminer_5
TestingGrounds_Exterior1_EventScript_RankExaminer_4:
# 265 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferGymProgress
# 266 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_GymProgress
# 267 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferStreak
# 268 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_StreakInfo
# 269 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_CanUpgradeSignatureTM
# 270 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, TRUE
	goto_if_eq TestingGrounds_Exterior1_EventScript_RankExaminer_8
TestingGrounds_Exterior1_EventScript_RankExaminer_7:
# 292 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_IsRankUpUnlocked
# 293 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, TRUE
	goto_if_eq TestingGrounds_Exterior1_EventScript_RankExaminer_11
TestingGrounds_Exterior1_EventScript_RankExaminer_10:
# 296 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_GYM_RANK, GYM_RANK_MAX
	goto_if_ge TestingGrounds_Exterior1_EventScript_RankExaminer_18
# 301 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_TestRankUpPrompt, MSGBOX_YESNO
# 302 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, YES
	goto_if_eq TestingGrounds_Exterior1_EventScript_RankExaminer_23
TestingGrounds_Exterior1_EventScript_RankExaminer_22:
# 307 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_RankExaminer_2:
# 258 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_DoublesInfo
	goto TestingGrounds_Exterior1_EventScript_RankExaminer_1

TestingGrounds_Exterior1_EventScript_RankExaminer_5:
# 263 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_OffTypeInfo
	goto TestingGrounds_Exterior1_EventScript_RankExaminer_4

TestingGrounds_Exterior1_EventScript_RankExaminer_8:
# 271 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_UpgradeOffer
# 272 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	message TestingGrounds_Exterior1_Text_UpgradeChoosePrompt
# 273 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitmessage
# 274 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferSignatureUpgradeChoices
# 275 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	dynmultichoice 0, 0, FALSE, 3, 0, DYN_MULTICHOICE_CB_NONE, gStringVar1, gStringVar2, TestingGrounds_Exterior1_Text_UpgradeLater
# 279 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, 0
	goto_if_eq TestingGrounds_Exterior1_EventScript_RankExaminer_13
# 279 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, 1
	goto_if_eq TestingGrounds_Exterior1_EventScript_RankExaminer_13
	goto TestingGrounds_Exterior1_EventScript_RankExaminer_7

TestingGrounds_Exterior1_EventScript_RankExaminer_11:
# 294 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_ExamUnlockedNote
	goto TestingGrounds_Exterior1_EventScript_RankExaminer_10

TestingGrounds_Exterior1_EventScript_RankExaminer_13:
# 280 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	copyvar VAR_0x8004, VAR_RESULT
# 281 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_UpgradeConfirmWarn, MSGBOX_YESNO
# 282 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, YES
	goto_if_eq TestingGrounds_Exterior1_EventScript_RankExaminer_20
	goto TestingGrounds_Exterior1_EventScript_RankExaminer_7

TestingGrounds_Exterior1_EventScript_RankExaminer_18:
# 297 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_MaxRank
# 298 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_RankExaminer_20:
# 283 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_UpgradeSignatureTM
# 284 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	playfanfare MUS_OBTAIN_TMHM
# 285 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	message TestingGrounds_Exterior1_Text_UpgradeReceived
# 286 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitfanfare
# 287 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	waitmessage
# 288 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_UpgradeDone
	goto TestingGrounds_Exterior1_EventScript_RankExaminer_7

TestingGrounds_Exterior1_EventScript_RankExaminer_23:
# 303 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_RankUp
# 304 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special GymChallenge_BufferRankInfo
# 305 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_RankedUp
	goto TestingGrounds_Exterior1_EventScript_RankExaminer_22


TestingGrounds_Exterior1_EventScript_ExpBlockerNpc::
# 317 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	lock
# 318 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	faceplayer
# 319 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	checkitem ITEM_EXP_BLOCKER
# 320 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, FALSE
	goto_if_eq TestingGrounds_Exterior1_EventScript_ExpBlockerNpc_2
# 324 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	goto_if_set I_EXP_GAIN_PAUSE_FLAG, TestingGrounds_Exterior1_EventScript_ExpBlockerNpc_3
# 327 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_ExpBlockerStateActive
TestingGrounds_Exterior1_EventScript_ExpBlockerNpc_1:
# 329 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_ExpBlockerNpc_2:
# 321 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_ExpBlockerIntro
# 322 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	giveitem ITEM_EXP_BLOCKER
# 323 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_ExpBlockerExplain
	goto TestingGrounds_Exterior1_EventScript_ExpBlockerNpc_1

TestingGrounds_Exterior1_EventScript_ExpBlockerNpc_3:
# 325 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_ExpBlockerStatePaused
	goto TestingGrounds_Exterior1_EventScript_ExpBlockerNpc_1


TestingGrounds_Exterior1_EventScript_RequestBoard::
# 338 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	lockall
# 339 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	fadescreen FADE_TO_BLACK
# 342 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special CommunityRequests_OpenBoard
# 343 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	releaseall
	end


TestingGrounds_Exterior1_EventScript_Visitor::
# 353 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	lock
# 354 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	faceplayer
# 355 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	setvar VAR_0x8004, REQUEST_GREET_THE_LEADER
# 356 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special CommunityRequests_GetRequestStatus
# 357 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	switch VAR_RESULT
# 358 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case REQUEST_STATUS_ACTIVE, TestingGrounds_Exterior1_EventScript_Visitor_3
# 367 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case REQUEST_STATUS_DONE, TestingGrounds_Exterior1_EventScript_Visitor_4
# 370 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_VisitorLocked
TestingGrounds_Exterior1_EventScript_Visitor_1:
# 372 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_Visitor_3:
# 359 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_VisitorGreeting
# 360 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special CommunityRequests_HasRoomForRewards
# 361 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, FALSE
	goto_if_eq TestingGrounds_Exterior1_EventScript_Visitor_7
# 366 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	call CommunityRequests_EventScript_PayReward
	goto TestingGrounds_Exterior1_EventScript_Visitor_1

TestingGrounds_Exterior1_EventScript_Visitor_4:
# 368 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_VisitorDone
	goto TestingGrounds_Exterior1_EventScript_Visitor_1

TestingGrounds_Exterior1_EventScript_Visitor_7:
# 362 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_RequestBagFull
# 363 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end


TestingGrounds_Exterior1_EventScript_OldMan::
# 381 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	lock
# 382 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	faceplayer
# 383 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	setvar VAR_0x8004, REQUEST_POTION_FOR_THE_OLD_MAN
# 384 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special CommunityRequests_GetRequestStatus
# 385 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	switch VAR_RESULT
# 386 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case REQUEST_STATUS_LOCKED, TestingGrounds_Exterior1_EventScript_OldMan_3
# 390 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case REQUEST_STATUS_ACTIVE, TestingGrounds_Exterior1_EventScript_OldMan_4
# 412 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case REQUEST_STATUS_DONE, TestingGrounds_Exterior1_EventScript_OldMan_5
TestingGrounds_Exterior1_EventScript_OldMan_1:
# 415 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_OldMan_3:
# 387 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_OldManAsk
# 388 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special CommunityRequests_UnlockRequest
# 389 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_OldManPosted
	goto TestingGrounds_Exterior1_EventScript_OldMan_1

TestingGrounds_Exterior1_EventScript_OldMan_4:
# 391 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	checkitem ITEM_POTION
# 392 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, FALSE
	goto_if_eq TestingGrounds_Exterior1_EventScript_OldMan_7
# 397 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_OldManHandOver, MSGBOX_YESNO
# 398 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, NO
	goto_if_eq TestingGrounds_Exterior1_EventScript_OldMan_10
# 403 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special CommunityRequests_HasRoomForRewards
# 404 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, FALSE
	goto_if_eq TestingGrounds_Exterior1_EventScript_OldMan_13
# 409 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	removeitem ITEM_POTION
# 410 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_OldManThanks
# 411 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	call CommunityRequests_EventScript_PayReward
	goto TestingGrounds_Exterior1_EventScript_OldMan_1

TestingGrounds_Exterior1_EventScript_OldMan_5:
# 413 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_OldManDone
	goto TestingGrounds_Exterior1_EventScript_OldMan_1

TestingGrounds_Exterior1_EventScript_OldMan_7:
# 393 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_OldManWaiting
# 394 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_OldMan_10:
# 399 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_OldManWaiting
# 400 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_OldMan_13:
# 405 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_RequestBagFull
# 406 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end


TestingGrounds_Exterior1_EventScript_RowdyTrainer::
# 425 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	lock
# 426 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	faceplayer
# 427 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	setvar VAR_0x8004, REQUEST_ROWDY_TRAINER
# 428 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special CommunityRequests_GetRequestStatus
# 429 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, REQUEST_STATUS_DONE
	goto_if_eq TestingGrounds_Exterior1_EventScript_RowdyTrainer_2
# 434 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, REQUEST_STATUS_LOCKED
	goto_if_eq TestingGrounds_Exterior1_EventScript_RowdyTrainer_5
# 439 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special CommunityRequests_GetRequestStep
# 440 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	switch VAR_RESULT
# 441 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case 0, TestingGrounds_Exterior1_EventScript_RowdyTrainer_9
# 444 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	case 1, TestingGrounds_Exterior1_EventScript_RowdyTrainer_10
# 448 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_RowdyStep3
# 449 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special CommunityRequests_HasRoomForRewards
# 450 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, FALSE
	goto_if_eq TestingGrounds_Exterior1_EventScript_RowdyTrainer_13
# 455 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	call CommunityRequests_EventScript_PayReward
TestingGrounds_Exterior1_EventScript_RowdyTrainer_7:
# 457 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_RowdyTrainer_2:
# 430 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_RowdyDone
# 431 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_RowdyTrainer_5:
# 435 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_RowdyLocked
# 436 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end

TestingGrounds_Exterior1_EventScript_RowdyTrainer_9:
# 442 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_RowdyStep1
# 443 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special CommunityRequests_AdvanceStep
	goto TestingGrounds_Exterior1_EventScript_RowdyTrainer_7

TestingGrounds_Exterior1_EventScript_RowdyTrainer_10:
# 445 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_RowdyStep2
# 446 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	special CommunityRequests_AdvanceStep
	goto TestingGrounds_Exterior1_EventScript_RowdyTrainer_7

TestingGrounds_Exterior1_EventScript_RowdyTrainer_13:
# 451 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_RequestBagFull
# 452 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	release
	end


TestingGrounds_Exterior1_EventScript_SupplyCrate::
# 466 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	lockall
# 467 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	msgbox TestingGrounds_Exterior1_Text_SupplyCrate, MSGBOX_YESNO
# 468 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	compare VAR_RESULT, YES
	goto_if_eq TestingGrounds_Exterior1_EventScript_SupplyCrate_2
TestingGrounds_Exterior1_EventScript_SupplyCrate_1:
# 471 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	releaseall
	end

TestingGrounds_Exterior1_EventScript_SupplyCrate_2:
# 469 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	giveitem ITEM_POTION
	goto TestingGrounds_Exterior1_EventScript_SupplyCrate_1


# 144 "data/maps/TestingGrounds_Exterior1/scripts.pory"
TestingGrounds_Exterior1_EventScript_GymAttendant_Movement_0:
# 144 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	walk_down
# 144 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	walk_down
	step_end

# 160 "data/maps/TestingGrounds_Exterior1/scripts.pory"
TestingGrounds_Exterior1_EventScript_GymAttendant_Movement_1:
# 160 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	walk_up
# 160 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	walk_up
	step_end

TestingGrounds_Exterior1_Text_GiftIntro::
# 475 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Ah, our new GYM LEADER!\p"
	.string "A LEADER needs POKéMON of the GYM's\n"
	.string "own type.\p"
	.string "Take these three {STR_VAR_1}-type\n"
	.string "partners!$"

TestingGrounds_Exterior1_Text_GiftReceived::
# 483 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "{PLAYER} received three\n"
	.string "{STR_VAR_1}-type POKéMON!$"

TestingGrounds_Exterior1_Text_GiftExplainRules::
# 488 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "At rank {STR_VAR_1}, you defend with\n"
	.string "{STR_VAR_3} POKéMON at level {STR_VAR_2}\l"
	.string "or below.\p"
	.string "Register teams with the TEAM KEEPER,\n"
	.string "then speak to the attendant to open\l"
	.string "your GYM!$"

TestingGrounds_Exterior1_Text_ReceivedSignatureTM::
# 497 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "{STR_VAR_2} was added to\n"
	.string "{PLAYER}'s TM MACHINE!$"

TestingGrounds_Exterior1_Text_ExplainSignatureTM::
# 502 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "That TM teaches {STR_VAR_3} -\n"
	.string "your GYM's signature move!\p"
	.string "Open the TM MACHINE from the menu\n"
	.string "to teach your unlocked TMs.\p"
	.string "Most cost GYM POINTS, but your\n"
	.string "signature move is always free!\p"
	.string "One day, outsider POKéMON that\n"
	.string "learn it may fight by your side.$"

TestingGrounds_Exterior1_Text_GiftPostGift::
# 513 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "How are your POKéMON doing?\p"
	.string "Defend your GYM well and it will\n"
	.string "grow in rank!$"

TestingGrounds_Exterior1_Text_GiftNoRoom::
# 519 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Oh! You don't have room for three\n"
	.string "more POKéMON.\p"
	.string "Come back when you've made space.$"

TestingGrounds_Exterior1_Text_OpenGymPrompt::
# 525 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Welcome, GYM LEADER {PLAYER}!\p"
	.string "Shall we open the GYM and accept\n"
	.string "challenges?$"

TestingGrounds_Exterior1_Text_ComeBackAnytime::
# 531 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Very well. Come back whenever\n"
	.string "you're ready.$"

TestingGrounds_Exterior1_Text_ChooseTeamSource::
# 536 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Who defends the GYM today?$"

TestingGrounds_Exterior1_Text_SourceParty::
# 540 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "CURRENT PARTY$"

TestingGrounds_Exterior1_Text_SourceCancel::
# 544 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "CANCEL$"

TestingGrounds_Exterior1_Text_TeamUnavailable::
# 548 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Hmm… I can't find any of that\n"
	.string "team's members.\p"
	.string "Have they been released or\n"
	.string "traded away?$"

TestingGrounds_Exterior1_Text_PickDefenders::
# 555 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Now, pick the {STR_VAR_3} POKéMON\n"
	.string "that will defend the GYM!$"

TestingGrounds_Exterior1_Text_DoublesReminder::
# 560 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Remember: at your rank, GYM\n"
	.string "battles are fought in pairs!\p"
	.string "Your first two picks will lead\n"
	.string "the battle together.$"

TestingGrounds_Exterior1_Text_DoublesInfo::
# 567 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "At your rank, all GYM battles\n"
	.string "are DOUBLE BATTLES.$"

TestingGrounds_Exterior1_Text_TeamWrongSize::
# 572 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Hmm… At rank {STR_VAR_1}, a LEADER\n"
	.string "defends with up to {STR_VAR_3} POKéMON.\p"
	.string "Please adjust your party and\n"
	.string "come back.$"

TestingGrounds_Exterior1_Text_TeamOverleveled::
# 579 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Hmm… At rank {STR_VAR_1}, your POKéMON\n"
	.string "must be level {STR_VAR_2} or below.\p"
	.string "Please adjust your party and\n"
	.string "come back.$"

TestingGrounds_Exterior1_Text_TeamWrongType::
# 586 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Hmm… This is a {STR_VAR_1}-type GYM.\p"
	.string "Your team may only include POKéMON\n"
	.string "of the GYM's type, or an outsider\l"
	.string "that knows your signature move.\p"
	.string "Please adjust your party and\n"
	.string "come back.$"

TestingGrounds_Exterior1_Text_GymIsOpen::
# 595 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "The GYM is now open!\p"
	.string "Three TRAINERS signed up to\n"
	.string "challenge you today.\p"
	.string "Here comes the first one!$"

TestingGrounds_Exterior1_Text_StreakPhraseCalm::
# 602 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "ATTENDANT: Mostly first-timers in\n"
	.string "line today.\p"
	.string "A calm day at the GYM, I'd say.$"

TestingGrounds_Exterior1_Text_StreakPhraseStirring::
# 608 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "ATTENDANT: A few familiar faces in\n"
	.string "line today…\p"
	.string "Some of them have clearly been\n"
	.string "training for this.$"

TestingGrounds_Exterior1_Text_StreakPhraseSpreading::
# 615 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "ATTENDANT: Word of your streak is\n"
	.string "spreading, LEADER.\p"
	.string "Serious TRAINERS are starting to\n"
	.string "line up outside.$"

TestingGrounds_Exterior1_Text_StreakPhrasePeak::
# 622 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "ATTENDANT: The line is full of\n"
	.string "hardened TRAINERS today!\p"
	.string "Every one of them has come to end\n"
	.string "your streak. Stay sharp!$"

TestingGrounds_Exterior1_Text_StrongChallengerWarning::
# 629 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "ATTENDANT: Careful, LEADER…\p"
	.string "This one has the look of a real\n"
	.string "contender!$"

TestingGrounds_Exterior1_Text_StreakInfo::
# 635 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Your current win streak:\n"
	.string "{STR_VAR_1} in a row.$"

TestingGrounds_Exterior1_Text_EarnedGymPoints::
# 640 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "{PLAYER} earned {STR_VAR_1} GYM POINTS!$"

TestingGrounds_Exterior1_Text_BarFilled::
# 644 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "ATTENDANT: Incredible!\p"
	.string "Your GYM's reputation has reached\n"
	.string "its peak for this rank!\p"
	.string "You may take the RANK-UP EXAM\n"
	.string "whenever you feel ready!$"

TestingGrounds_Exterior1_Text_ExamAvailable::
# 652 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "ATTENDANT: Remember, the RANK-UP\n"
	.string "EXAM awaits whenever you're ready!$"

TestingGrounds_Exterior1_Text_ChallengerDefeated::
# 657 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "{STR_VAR_1}: Aww… You really are\n"
	.string "the GYM LEADER!\p"
	.string "I'll train harder and challenge\n"
	.string "you again!$"

TestingGrounds_Exterior1_Text_NextChallengerArrived::
# 664 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "ATTENDANT: Well defended!\p"
	.string "Your POKéMON have been healed.\n"
	.string "The next challenger has arrived!$"

TestingGrounds_Exterior1_Text_LeaderLost::
# 670 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "{STR_VAR_1}: I… I won?!\p"
	.string "I beat a GYM LEADER! Wait till\n"
	.string "everyone hears about this!$"

TestingGrounds_Exterior1_Text_GymClosedAfterLoss::
# 676 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "ATTENDANT: Oh no…\p"
	.string "We'll close the GYM for today.\n"
	.string "Rest up and try again tomorrow!$"

TestingGrounds_Exterior1_Text_GymDayComplete::
# 682 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "ATTENDANT: That was the last\n"
	.string "challenger for today!\p"
	.string "You defended your GYM perfectly,\n"
	.string "LEADER {PLAYER}!\p"
	.string "Keep this up and the GYM's rank\n"
	.string "will surely rise!$"

TestingGrounds_Exterior1_Text_TeamKeeperIntro::
# 691 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Hello, LEADER! I keep your\n"
	.string "registered GYM TEAMS.\p"
	.string "A team remembers its members even\n"
	.string "if you move them around the PC.$"

TestingGrounds_Exterior1_Text_ChooseTeam::
# 698 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Which team shall we look at?$"

TestingGrounds_Exterior1_Text_Team1::
# 702 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "TEAM 1$"

TestingGrounds_Exterior1_Text_Team2::
# 706 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "TEAM 2$"

TestingGrounds_Exterior1_Text_Team3::
# 710 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "TEAM 3$"

TestingGrounds_Exterior1_Text_Team4::
# 714 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "TEAM 4$"

TestingGrounds_Exterior1_Text_Team5::
# 718 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "TEAM 5$"

TestingGrounds_Exterior1_Text_TeamMenuExit::
# 722 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "EXIT$"

TestingGrounds_Exterior1_Text_TeamEmpty::
# 726 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "TEAM {STR_VAR_2} is empty.$"

TestingGrounds_Exterior1_Text_TeamMembers::
# 730 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "TEAM {STR_VAR_2}:\p"
	.string "{STR_VAR_1}$"

TestingGrounds_Exterior1_Text_TeamActionRegister::
# 735 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "REGISTER PARTY$"

TestingGrounds_Exterior1_Text_TeamActionClear::
# 739 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "CLEAR TEAM$"

TestingGrounds_Exterior1_Text_TeamActionBack::
# 743 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "BACK$"

TestingGrounds_Exterior1_Text_TeamRegistered::
# 747 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Done! Your current party is now\n"
	.string "TEAM {STR_VAR_2}.$"

TestingGrounds_Exterior1_Text_TeamCleared::
# 752 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "TEAM {STR_VAR_2} has been cleared.$"

TestingGrounds_Exterior1_Text_TeamKeeperBye::
# 756 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Come back anytime, LEADER!$"

TestingGrounds_Exterior1_Text_RankInfo::
# 760 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "EXAMINER: You preside at rank\n"
	.string "{STR_VAR_1}, LEADER.\p"
	.string "Your GYM team may have {STR_VAR_3}\n"
	.string "POKéMON of level {STR_VAR_2} or below.$"

TestingGrounds_Exterior1_Text_SignatureMoveInfo::
# 767 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Your GYM's signature move is\n"
	.string "{STR_VAR_1}.$"

TestingGrounds_Exterior1_Text_OffTypeInfo::
# 772 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "At your rank, one team member may\n"
	.string "be an outsider type…\p"
	.string "…as long as it knows {STR_VAR_1},\n"
	.string "your signature move!$"

TestingGrounds_Exterior1_Text_GymProgress::
# 779 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "You lead a {STR_VAR_1}-type GYM.\p"
	.string "Reputation this rank:\n"
	.string "{STR_VAR_2} of {STR_VAR_3} points.$"

TestingGrounds_Exterior1_Text_UpgradeOffer::
# 785 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Ah, one more thing, LEADER!\p"
	.string "A GYM of your rank may adopt a\n"
	.string "stronger signature move!$"

TestingGrounds_Exterior1_Text_UpgradeChoosePrompt::
# 791 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Which move shall represent\n"
	.string "your GYM?$"

TestingGrounds_Exterior1_Text_UpgradeLater::
# 796 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "LATER$"

TestingGrounds_Exterior1_Text_UpgradeConfirmWarn::
# 800 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Once you adopt the new move, only\n"
	.string "it will count as your signature.\p"
	.string "Outsiders that know only the old\n"
	.string "move will no longer qualify.\p"
	.string "Proceed?$"

TestingGrounds_Exterior1_Text_UpgradeReceived::
# 808 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "{STR_VAR_2} was added to\n"
	.string "{PLAYER}'s TM MACHINE!$"

TestingGrounds_Exterior1_Text_UpgradeDone::
# 813 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "{STR_VAR_3} is now your GYM's\n"
	.string "signature move - teach it free\l"
	.string "from your TM MACHINE!\p"
	.string "The move you passed on was\n"
	.string "unlocked too - it just costs\l"
	.string "GYM POINTS.\p"
	.string "May it strike fear into every\n"
	.string "challenger!$"

TestingGrounds_Exterior1_Text_ExamUnlockedNote::
# 824 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Your reputation is at its peak!\n"
	.string "The RANK-UP EXAM awaits you.$"

TestingGrounds_Exterior1_Text_MaxRank::
# 829 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "You stand at the very top,\n"
	.string "LEADER. There is no higher rank!$"

TestingGrounds_Exterior1_Text_TestRankUpPrompt::
# 834 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "(TEST) Advance to the next rank\n"
	.string "right now?$"

TestingGrounds_Exterior1_Text_RankedUp::
# 839 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Congratulations, LEADER!\p"
	.string "You now preside at rank {STR_VAR_1}!\p"
	.string "New rules: {STR_VAR_3} POKéMON of\n"
	.string "level {STR_VAR_2} or below.$"

TestingGrounds_Exterior1_Text_ExpBlockerIntro::
# 846 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Testing EXP curves, are we?\p"
	.string "Here, take this EXP. BLOCKER.$"

TestingGrounds_Exterior1_Text_ExpBlockerExplain::
# 851 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Use it from the KEY ITEMS pocket to\n"
	.string "pause or resume EXP for your whole\l"
	.string "party.\p"
	.string "Your POKéMON keep earning EVs even\n"
	.string "while EXP is paused.$"

TestingGrounds_Exterior1_Text_ExpBlockerStatePaused::
# 859 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "EXP gain is currently PAUSED.\p"
	.string "Use the EXP. BLOCKER again to resume\n"
	.string "it. EVs are still being earned.$"

TestingGrounds_Exterior1_Text_ExpBlockerStateActive::
# 865 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "EXP gain is currently ACTIVE.\p"
	.string "Use the EXP. BLOCKER to pause it\n"
	.string "whenever you like.$"

TestingGrounds_Exterior1_Text_RequestBagFull::
# 871 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Your BAG is too full to accept the\n"
	.string "reward.\p"
	.string "Come back once you've made some\n"
	.string "room!$"

TestingGrounds_Exterior1_Text_VisitorGreeting::
# 878 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Oh! You really are the new GYM\n"
	.string "LEADER!\p"
	.string "I put a note on the GYM's board just\n"
	.string "hoping you'd read it.\p"
	.string "Meeting you in person is more than\n"
	.string "I hoped for. Thank you!$"

TestingGrounds_Exterior1_Text_VisitorDone::
# 887 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "I'm telling everyone back home that\n"
	.string "I met the GYM LEADER!$"

TestingGrounds_Exterior1_Text_VisitorLocked::
# 892 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Is this GYM open to visitors?\p"
	.string "I'll wait here until someone says\n"
	.string "otherwise.$"

TestingGrounds_Exterior1_Text_OldManAsk::
# 898 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "My POKéMON took a bad fall and I\n"
	.string "can't make it to the MART.\p"
	.string "Could the GYM spare a POTION for an\n"
	.string "old man?$"

TestingGrounds_Exterior1_Text_OldManPosted::
# 905 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "I'll pin my request to the GYM's\n"
	.string "board so you don't forget.$"

TestingGrounds_Exterior1_Text_OldManWaiting::
# 910 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "A POTION. Any POTION. I'd be so\n"
	.string "grateful.$"

TestingGrounds_Exterior1_Text_OldManHandOver::
# 915 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Oh, is that a POTION?\p"
	.string "Would you part with it?$"

TestingGrounds_Exterior1_Text_OldManThanks::
# 920 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Bless you, LEADER.\p"
	.string "A GYM that looks after its\n"
	.string "neighbors is a GYM worth having.$"

TestingGrounds_Exterior1_Text_OldManDone::
# 926 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "My partner is back on its feet,\n"
	.string "thanks to you.$"

TestingGrounds_Exterior1_Text_RowdyLocked::
# 931 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "What are you looking at?$"

TestingGrounds_Exterior1_Text_RowdyStep1::
# 935 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "So you're the LEADER around here.\p"
	.string "This city's TRAINERS are soft. Someone\n"
	.string "had to say it.$"

TestingGrounds_Exterior1_Text_RowdyStep2::
# 941 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "Still here? Fine.\p"
	.string "Maybe I've been picking fights with\n"
	.string "the wrong people.$"

TestingGrounds_Exterior1_Text_RowdyStep3::
# 947 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "All right, all right. I'll apologize\n"
	.string "to the folks by the gate.\p"
	.string "Tell the city the GYM sorted it out.$"

TestingGrounds_Exterior1_Text_RowdyDone::
# 953 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "I've been keeping it civil. Mostly.$"

TestingGrounds_Exterior1_Text_SupplyCrate::
# 957 "data/maps/TestingGrounds_Exterior1/scripts.pory"
	.string "The GYM's supply crate. There are\n"
	.string "POTIONS inside.\p"
	.string "Take one?$"
