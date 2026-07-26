CommunityRequests_EventScript_PayReward::
# 12 "data/scripts/community_requests.pory"
	special CommunityRequests_CompleteRequest
# 15 "data/scripts/community_requests.pory"
	compare VAR_RESULT, FALSE
	goto_if_eq CommunityRequests_EventScript_PayReward_2
# 18 "data/scripts/community_requests.pory"
	playfanfare MUS_OBTAIN_ITEM
# 19 "data/scripts/community_requests.pory"
	compare VAR_0x8006, 0
	goto_if_gt CommunityRequests_EventScript_PayReward_5
# 22 "data/scripts/community_requests.pory"
	message CommunityRequests_Text_RequestPaid
CommunityRequests_EventScript_PayReward_4:
# 24 "data/scripts/community_requests.pory"
	waitfanfare
# 25 "data/scripts/community_requests.pory"
	waitmessage
	return

CommunityRequests_EventScript_PayReward_2:
	return

CommunityRequests_EventScript_PayReward_5:
# 20 "data/scripts/community_requests.pory"
	message CommunityRequests_Text_RequestPaidWithItems
	goto CommunityRequests_EventScript_PayReward_4


CommunityRequests_Text_RequestPaid::
# 29 "data/scripts/community_requests.pory"
	.string "{PLAYER} received ¥{STR_VAR_1} from\nthe grateful citizens!$"

CommunityRequests_Text_RequestPaidWithItems::
# 33 "data/scripts/community_requests.pory"
	.string "{PLAYER} received ¥{STR_VAR_1} and\n{STR_VAR_2} from the grateful citizens!$"
