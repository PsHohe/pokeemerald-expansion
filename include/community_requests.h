#ifndef GUARD_COMMUNITY_REQUESTS_H
#define GUARD_COMMUNITY_REQUESTS_H

#include "constants/community_requests.h"

// Request state
u32 CommunityRequests_GetStatus(u32 id);
u32 CommunityRequests_GetStep(u32 id);
void CommunityRequests_UnlockForRank(u32 rank);

// Script specials (request id in gSpecialVar_0x8004)
void CommunityRequests_OpenBoard(void);
void CommunityRequests_GetRequestStatus(void);
void CommunityRequests_GetRequestStep(void);
void CommunityRequests_UnlockRequest(void);
void CommunityRequests_AdvanceStep(void);
void CommunityRequests_HasRoomForRewards(void);
void CommunityRequests_CompleteRequest(void);

#endif // GUARD_COMMUNITY_REQUESTS_H
