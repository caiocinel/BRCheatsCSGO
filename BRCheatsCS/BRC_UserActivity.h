// Todos os id de cada tipo de atividade, use em BRC_StartUserActivity() com a devida atividade.
enum ActivityId : int
{
	ActivityId_Offline,
	ActivityId_Loader,
	ActivityId_PBBrazil,
	ActivityId_PBPrivate,
	ActivityId_CSGO,
	ActivityId_MuSeason4,
	ActivityId_FallGuys,
	ActivityId_CreativeDestruction,
	ActivityId_PUBGLITE,
	ActivityId_WarfaceBR,
	ActivityId_IronSight,
	ActivityId_DeadByDaylight,
	ActivityId_Spellbreak,
	ActivityId_AmongUs,
	ActivityId_BlackSquad,
	ActivityId_RogueCompany,
	ActivityId_InsurgencySandstorm,
	ActivityId_Fortnite,
	ActivityId_ApexLegends
};

// Verifica se o acesso é válido (se está usando dentro de 10 minutos após selecionar a opção no loader)
bool BRC_IsValidTimestampAccess();

// Pega o nome do usuário, se o acesso for inválido retorna string vazia ""
const char* BRC_GetUserName();

// Inicia o sistema de atividade
void BRC_StartUserActivity(ActivityId GameId);