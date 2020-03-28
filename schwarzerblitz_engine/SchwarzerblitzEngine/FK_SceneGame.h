#ifndef FK_SCENEGAME_CLASS
#define FK_SCENEGAME_CLASS

#include<irrlicht.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cmath>
#include<mutex>
#include<condition_variable>
#include<atomic>

#include <Windows.h>
#include <Winuser.h>

#include "XEffects.h"
#include "FK_SceneViewUpdater.h"
#include "FK_MoveFileParser.h"
#include "FK_Character.h"
#include "FK_InputReceiver.h"
#include "FK_WorldCollisionCallback.h"
#include "FK_HUDManager_font.h"
#include "FK_Stage.h"
#include "FK_SceneWithInput.h"
#include "FK_AIManager.h"
#include "FK_Announcer.h"
#include "FK_SoundManager.h"
#include "FK_PixelShaderStageCallback.h"
#include "FK_PixelShaderSceneGameCallback.h"
#include "FK_InGameMenu.h"
#include "FK_MoveListPanel.h"
#include "FK_FreeMatchStatistics.h"
#include "FK_Tutorial.h"

#include <SFML\Audio.hpp>

using namespace irr;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace fk_engine{

	class FK_SceneGame : public FK_SceneWithInput{
	public:

		struct FK_FreeCameraParameters {
			f32 deltaCameraAnglePerSecond = 60.f;
			f32 deltaCameraDistancePerSecond = 40.f;
			f32 deltaCameraOffsetYPerSecond = 35.f;
			f32 currentCameraAngle = 0.f;
			f32 currentCameraDistance = 0.f;
			f32 currentCameraOffsetY = 0.f;
			f32 currentCameraTargetOffsetY = 0.f;
			f32 minimalCameraDistance = 20.f;
			f32 maximalCameraDistance = 200.f;
			f32 minimalYOffset = -50.f;
			f32 maximalYOffset = 75.f;
			u32 lastCameraTimeUpdateMs = 0;
			s32 targetIndex = 0;
			u32 snapEffectDurationMs = 150;
		};

		struct FK_AdditionalSceneGameOptions{
			s32 numberOfArcadeMatches = -1;
			s32 numberOfRounds = -1;
			s32 numberOfRoundsPlayer1MustWin = -1;
			s32 numberOfRoundsPlayer2MustWin = -1;
			u32 roundTimerInSeconds = 99;
			bool overrideTimer = false;
			f32 lifeMultiplierPlayer1 = 1.0f;
			f32 lifeMultiplierPlayer2 = 1.0f;
			bool allowTriggers = true;
			bool regenerateLifePlayer1OnNewRound = true;
			bool regenerateLifePlayer2OnNewRound = true;
			f32 continuousDamagePlayer1perSecond = 0.0f;
			f32 continuousDamagePlayer2perSecond = 0.0f;
			s32 triggerRegenerationTimeMSPlayer1 = 0;
			s32 triggerRegenerationTimeMSPlayer2 = 0;
			bool showRoundWinScreen = true;
			bool allowRingout = true;
			bool saveReplay = false;
			bool playReplay = false;
			f32 receivedPhysicalDamageMultiplierPlayer1 = 1.f;
			f32 receivedPhysicalDamageMultiplierPlayer2 = 1.f;
			f32 receivedBulletDamageMultiplierPlayer1 = 1.f;
			f32 receivedBulletDamageMultiplierPlayer2 = 1.f;
			std::string player1MovesetIdentifier = std::string();
			std::string player2MovesetIdentifier = std::string();
			bool allowNonStandardArcadeBosses = false;
			bool allowArcadeSpecialMatches = false;
			bool allowArcadeClimaticBattle = true;
			f32 player1InitialRoundLifeMultiplier = -1.0f;
			f32 player2InitialRoundLifeMultiplier = -1.0f;
			bool allowContinue = true;
			bool playArcadeEnding = true;
		};
	protected:
		// collectible
		struct FK_Collectible {
			IAnimatedMeshSceneNode* node = NULL;
			std::string meshFileName;
			core::vector3d<f32> position = core::vector3d<f32>(0.f, 0.f, 0.f);
			core::vector3d<f32> scale = core::vector3d<f32>(1.f, 1.f, 1.f);
			bool canBePickedByPlayer1 = true;
			bool canBePickedByPlayer2 = false;
		};
	protected:
		const f32 additionalYunitsPerScaleUnit = 40.f;
		const u32 throwMaxReactionTimeMs = 150;
		const FK_Reaction_Type throwEscapeReactionPerformer = FK_Reaction_Type::StrongMedium;
		const FK_Reaction_Type throwEscapeReactionTarget = FK_Reaction_Type::StrongMedium;
		const FK_Reaction_Type weakTriggerReaction = FK_Reaction_Type::StrongHigh;
		const FK_Reaction_Type strongTriggerReaction = FK_Reaction_Type::StrongFlight;
		core::vector3df player2startingPosition = core::vector3df(50, 0, 60);
		core::vector3df player1startingPosition = core::vector3df(50, 0, -60);
		const f32 sceneGravity = -550.0f;
		const f32 jumpGravity = -1400.0f; 
		const u32 impactEffectDuration = 300;
	protected:
		// methods to be overloaded from subclasses
		virtual void processEndOfRoundStatistics();
		virtual void applyDamageToPlayer(FK_Character* player, f32 damage, bool affectObjects = true);
		virtual void updateAdditionalSceneLogic(u32 delta_t_ms);
		virtual void drawAdditionalHUDItems(f32 delta_t_s);
		virtual void drawAdditionalHUDOverlay(f32 delta_t_s);
		virtual void setupAdditionalVariables();
		virtual void setupAdditionalParametersAfterLoading();
		virtual std::wstring getSceneNameForLoadingScreen();
		virtual void processNewRoundStart();
		virtual void processRoundEnd();
		virtual void processMatchEnd();
		virtual bool showRoundIndicatorsHUD();
		virtual void updateGenericPlayerInput(FK_Character* character, FK_InputBuffer* buffer, s32 joypadId, u32 delta_t_ms, bool &newMove);
		virtual void updatePlayer1Input(u32 delta_t_ms, bool& newMove);
		virtual void updatePlayer2Input(u32 delta_t_ms, bool& newMove);
		virtual void setupResourcesPaths(std::string new_player1path, std::string new_player2path, std::string new_arenaPath);
		virtual void setupInputForPlayers();
		// draw loading screen while loading resources the beginning of the match
		virtual void loadLoadingTextures();
		virtual void drawLoadingScreen(f32 completionPercentage);
		/* impact effect */
		virtual void setupImpactEffect(FK_Reaction_Type reaction, s32 playerId = 1);
		/* pause */
		virtual std::vector<std::string> getPauseMenuItems();
		virtual void updatePauseMenu();
		virtual bool canUpdateInput();
		virtual bool canPauseGame();
		virtual void drawPauseMenu();
		virtual void pauseGame();
		virtual void activatePauseMenu();
		virtual void resumeGame();
		/* character pressed buttons */
		virtual void updateCharactersPressedButtons(u32 &pressedButtonsPlayer1, u32 &pressedButtonsPlayer2);
		/* throw */
		virtual void cancelThrow();
		/* restart match*/
		virtual void restartMatch();
		/* dispose */
		virtual void disposeAdditionalVariables();
		/* move list */
		virtual void showMoveList(bool player1MoveList = true);
		/* end of round */
		virtual int getRoundWinner(bool updateWins = false);
		virtual int getMatchWinner();
		virtual bool isEndOfRound();
		virtual bool isEndOfMatch();
		/* pause menu */
		virtual void resetInputTimeForPauseMenu();
		/* hud draw call for moves after successful throw */
		virtual void drawHUDThrowName(FK_Character* performer);
		/* AI settings */
		virtual bool updateAIInput(u32 delta_t_ms, u32 playerID);
		/* identifiers for sound */
		virtual std::string getEndOfRoundSoundIdentifier(const s32& matchWinnerId, const bool& perfectFlag);
		virtual void drawKOtext(const s32& matchWinnerId, const bool &perfectFlag);
		/* timer update */
		virtual void updateRoundTimer(u32 delta_t_ms);
		/* perform additional game over routines */
		virtual void processAdditionalGameOverRoutine();
		/* check for records */
		virtual bool checkForNewRecord();
		/* setup hud */
		virtual void setupHUD();
		// helper functions
		virtual void updateInputForPauseMenuAndSkipOptions();
		/* hitflags (void if not in Training mode) */
		virtual void addMoveHitFlag(ISceneNode* hitboxNode, FK_Hitbox* hitboxData,
			s32 frameAdvantage, s32 cancelAdvantage, bool showframeAdvantage);
		/* frame advantage*/
		virtual bool calculateFrameAdvantage(s32& frameAdvantage, s32& cancelAdvantage,
			FK_Hitbox * hitbox, f32 hitstunMultiplier, FK_Character* attacker, FK_Character* defender,
			bool guardBreak, bool counterAttack, bool defenderHasArmor);
	public:
		FK_SceneGame();
		virtual ~FK_SceneGame() {};
		void setup(IrrlichtDevice *newDevice,
			core::array<SJoystickInfo> new_joypadInfo,
			FK_Options* newOptions,
			std::string new_player1filename, 
			std::string new_player2filename, 
			FK_Character::FK_CharacterOutfit player1Outfit,
			FK_Character::FK_CharacterOutfit player2Outfit,
			std::string new_arenaPath,
			FK_SceneType currentSceneType,
			FK_AdditionalSceneGameOptions new_additionalOptions,
			bool player1AI = false, bool player2AI = false,
			int player1AILevel = 0, int player2AILevel = 0);
		void initialize();
		void update();
		virtual void dispose();
		virtual bool isRunning();
		FK_Scene::FK_SceneType nameId();
		f32* getSceneParametersForShader(FK_SceneShaderTypes shaderType);
		s32 getWinnerId();
		void setMatchStatistics(FK_FreeMatchStatistics new_matchStatistics);
		FK_FreeMatchStatistics getMatchStatistics();
	protected:
		// create render target textures
		void createRenderTargetTextures();
		void updateRenderTargetQuality(u32 frameDeltaTimeMs);
		void increaseRenderTextureQuality();
		void decreaseRenderTextureQuality();
		core::vector2df getRenderTargetScaleFactor();
		video::ITexture* getRenderTarget();
		s32 getRenderTargetQuality();
		void draw(f32 frameDeltaTimeS);
		// common methods
		void setupIrrlichtDevice();
		void setupArena();
		void setupCharacters(f32 &progress);
		void setupPlayer1Character(f32 &progress);
		void setupPlayer2Character(f32 &progress);
		void setupGameObjects();
		void setupCharacterArenaInteraction();
		void setupLights();
		void setupShaders();
		void setupCamera();
		void setupGameStates();
		void setupAnnouncer();
		void setupBattleSoundManager();
		void addCharacterSoundsToSoundManager();
		void addSoundsFromDirectoryToSoundManager(std::string directory, std::string extension, std::string prefix);
		bool playHitSounds(FK_Reaction_Type reaction, std::string playerTag, FK_Character* character);
		void setupMoveLists(f32 &progress);
		void setupMoveListsIconSetsBasedOnInput();
		void setupTutorial();
		void setupPauseMenu();
		// names for loading screen
		void drawPlayerNamesForLoadingScreen();
		void drawLoadingScreenProgressBar(f32 progress);
		std::string getCharacterName(std::string characterPath);
		// update characters
		void updateCharacters(f32 frameDeltaTime = 0);
		void updateBullets();
		void updateCharactersTransformations();
		void updateCharactersRingoutInteractions();
		void updateCamera(f32 delta_t_s = 0);
		void updateLights();
		void updateStage(u32 delta_t_ms = 0);
		void updateCollisions();
		void updateBulletCollisions();
		void updateInput(u32 delta_t_ms = 0);
		void updateInputReplay(u32 delta_t_ms = 0);
		void updateGameStates(u32 delta_t_ms);
		void updateCharacterArenaInteraction();
		void updateContinue(u32 delta_t_ms);
		void updateContinueInput();
		void updateTutorial(u32 inputButtons);
		void updateMoveListPanel();
		void updateTriggerRegeneration(bool player1WasHit, bool player2WasHit);
		void setupContinue();
		void resetInitialCharacterPositions();
		void drawHUD(f32 frameDeltaTime);
		bool checkDamageCollision(FK_Character* attackingPlayer, FK_Character* targetPlayer, 
			FK_Reaction_Type &reactionToApply, bool &guardBreakTrigger, f32 &hitDirection, f32 &hitstunMultiplier,
			FK_Move* &attemptedThrowMove);
		bool checkBulletDamageCollision(FK_Bullet* attackingBullet, FK_Character* bulletOwner, FK_Character* targetPlayer,
			FK_Reaction_Type &reactionToApply, f32 &hitDirection, f32 &hitstunMultiplier);
		f32 getDamageScalingFactor(f32 combo_damage, FK_Character* targetPlayer);
		// throws
		void updateThrowAttempt(u32 delta_t_ms);
		bool applySuccessfulThrow();
		bool isProcessingThrow();
		void resetThrowAttempt();
		bool checkNodeCollision(ISceneNode* one, ISceneNode* two);
		bool isStrongReaction(FK_Reaction_Type reaction);
		bool isPaused();
		bool isProcessingRingout();
		void resetRound();
		/* game logic (win, lose, new round...)*/
		bool isFinalRound();
		int getCurrentRoundNumber();
		bool canUpdateRoundTimer();
		bool isTimeUp();
		/* HUD flags */
		bool showRoundText();
		bool showFightText();
		bool showKOText();
		bool showWinText();
		bool showNewRecordText();
		bool showContinueText();
		bool showGameOverText();
		bool showHUD();
		/* trigger timer flag */
		bool isTriggerComboActive();
		void activateTriggerComboEffect();
		void cancelTriggerComboEffect();
		virtual void updateTriggerComboEffect(u32 frameDelta, bool newMovePlayer1, bool newMovePlayer2);
		/* counter attack SFX*/
		void updateCounterattackEffects(u32 frameDelta);
		void activateCounterattackEffects();
		void cancelCounterattackEffects();
		/* additional conditions */
		void updatePlayersPoisonEffect(f32 frameDelta_s);
		void updatePlayersTriggerRegen(u32 frameDelta);
		/* impact effect */
		bool isUpdatingImpactEffect();
		void updateImpactEffect(u32 frameDelta);
		/* replay */
		void initializeReplayStorage(u32 randomSeed);
		void openReplayFile(u32& randomSeed);
		void storeReplayData();
		void readReplayData(bool& timeInformationRead);
		bool isPlayingReplay();
		/* AI */
		void setupAI();
		void resetAI();
		void deleteAI();
		u32 getAIButtonPressed(u32 playerID);
		u32 getAIButtonPressedThrowDefense(u32 playerID);
		/* additional objects lost by characters */
		void createBreakableObjectProp(s32 ownerId,
			FK_Character::FK_CharacterAdditionalObject objectToAttach, IAnimatedMeshSceneNode* nodeToCopy, 
			std::string meshFileName = std::string());
		void updateProps(f32 delta_t_s);
		/* intro */
		bool isProcessingIntro();
		/* check if resources are being loaded */
		bool isLoading();
		/* check if move list is shown */
		bool isShowingMoveList();
		/* update save data at the end of every round */
		void updateSaveFileData();
		/* get elapsed time */
		u32 getElapsedTimeMS();
		/* get sound manager */
		FK_SoundManager* getSoundManager();
		/* get additional options */
		FK_AdditionalSceneGameOptions getAdditionalOptions();
		/* update free camera position */
		void activateFreeCameraMode();
		void updateFreeCameraPosition(u32 pressedButtons, u32 delta_t_ms);
		void setFreeCameraPosition(f32 &distance, f32 angle, f32 offsetY, f32 targetOffsetY = 0.f);
		void saveFreeCameraScreenshot();
	protected:
		/* scaling damage array */
		std::vector<f32> damageScaling;
		/* characters */
		FK_Character* player1;
		FK_Character* player2;
		FK_SceneViewUpdater * cameraManager;
		/* light effects */
		EffectHandler* lightManager;
		E_FILTER_TYPE shadowFilterType;
		E_SHADOW_MODE shadowModeCharacters;
		E_SHADOW_MODE shadowModeStage;
		/* arena node */
		FK_Stage* stage;
		/* HUD */
		FK_HUDManager_font *hudManager;
		/* skipIntro flag */
		bool skipIntro;
		/* AI parameters */
		bool player1isAI;
		bool player2isAI;
		/* states variables*/
		bool processingKOText;
		bool processingRoundText;
		bool processingFightText;
		bool processingEndOfRound;
		bool processingSceneEnd;
		bool processingContinue;
		bool processingGameOver;
		bool processingWinText;
		bool processingRecordText;
		bool isGameOver;
		bool showContinueTextFlag;
		bool timeupRoundEnd;
		u32 processingDurationMs;
		u32 processingKOTextDurationMs;
		u32 processingWinTextDurationMs;
		u32 processingIntervalMs;
		u32 processingTimeCounterMs;
		u32 processingContinueTextDurationMs;
		int winnerId;
		bool newRoundFlag;
		bool abortMatch;
		/* player life multiplier */
		f32 player1LifeMultiplier;
		f32 player2LifeMultiplier;
		int numberOfRounds;
		int numberOfRoundsPlayer1;
		int numberOfRoundsPlayer2;
		s32 roundTimerMs;
		s32 totalMatchTimeMs;
		u32 startRoundTimerMs;
		/* application path */
		std::string stagesPath;
		std::string charactersPath;
		/* config file names and paths */
		std::string player1_configPath;
		std::string player2_configPath;
		std::string arena_configPath;
		/* additional optionsfor various match conditions */
		FK_AdditionalSceneGameOptions additionalOptions;
		/* player outfit */
		FK_Character::FK_CharacterOutfit player1Outfit;
		FK_Character::FK_CharacterOutfit player2Outfit;
		/* pause menu */
		FK_InGameMenu* pauseMenu;
		/* AI managers */
		FK_AIManager* player1AIManager;
		FK_AIManager* player2AIManager;
		int player1AILevel;
		int player2AILevel;
		/* number of won matches*/
		FK_FreeMatchStatistics matchStatistics;
		/* player variables*/
		int numberOfPlayer1Wins;
		int numberOfPlayer2Wins;
		int currentRoundNumber;
		/* intro booleans */
		bool processingIntroPlayer1;
		bool processingIntroPlayer2;
		bool processingIntroStage;
		u32 introPhaseCounter;
		u32 lastSkipInputTime;
		/* bullet counters */
		s32 player1NumberOfBulletCounters;
		s32 player2NumberOfBulletCounters;
		/* time information */
		u32 then;
		u32 now;
		u32 nowReal;
		u32 thenReal;
	private:
		/* voice clips path */
		std::string voiceClipsPath;
		/* scene camera & scene manager */
		ICameraSceneNode * camera;
		ICameraSceneNode * freeCamera;
		FK_FreeCameraParameters freeCameraParameters;
		bool freeCameraModeActive;
		bool freeCameraSnap;
		bool freeCameraCanTakePicture;
		u32 freeCameraSnapCounter;
		//ICameraSceneNode * player1camera;
		//ICameraSceneNode * player2camera;
		/* collision manager with the arena*/
		FK_WorldCollisionCallback *player1CollisionCallback;
		FK_WorldCollisionCallback *player2CollisionCallback;
		/* additional time information */
		u32 startingTime;
		s32 matchStartingTime;
		s32 oldFps;
		u32 cycleId;
		/* pause variables */
		bool pause;
		bool pauseButtonDown;
		/* trigger */
		bool triggerComboActiveEffect;
		u32 triggerTimer;
		/* output file for replay */
		std::ofstream replayFileOut;
		std::ifstream replayFileIn;
		bool playReplay;
		bool saveReplay;
		std::deque<u32> player1ReplayBuffer;
		std::deque<u32> player2ReplayBuffer;
		u32 player1ReplayAllButtonsPressed;
		u32 player2ReplayAllButtonsPressed;
		bool player1ReplayBufferModified;
		bool player2ReplayBufferModified;
		bool replayTimeInformationRead;
		/* BGM */
		sf::Music stage_bgm;
		/* Announcer */
		FK_Announcer* announcer;
		bool canPlayAnnouncerVoice;
		/* Battle sounds */
		FK_SoundManager* battleSoundManager;
		/* backup variables for trigger sound effect */
		bool player1OldTriggerStatus;
		bool player2OldTriggerStatus;
		bool player1OldImpactCancelStatus;
		bool player2OldImpactCancelStatus;
		/* ringout bools */
		bool player1ringoutFlag;
		bool player2ringoutFlag;
		bool player1SplashEffectPlayed;
		bool player2SplashEffectPlayed;
		/* loading completion */
		std::atomic<f32> loadingPercentage;
		bool announcerLoaded;
		bool soundsLoaded;
		bool charactersLoaded;
		bool stageLoaded;
		bool hudLoaded;
		std::atomic<bool> allLoaded;
		/* loading screen variables*/
		video::ITexture* player1Preview;
		video::ITexture* player2Preview;
		std::string player1NameLoadingScreen;
		std::string player2NameLoadingScreen;
		video::ITexture* stageTex;
		gui::IGUIFont *font;
		gui::IGUIFont *textFont;
		gui::IGUIFont *titleFont;
		/* multi-threading mutex for safe use */
		std::mutex m_threadMutex;
		std::condition_variable m_thread_cv;
		/* callback for various shaders */
		std::map<std::string, FK_PixelShaderSceneGameCallback*> scenePixelShaderCallback;
		/* stage shaders */
		std::vector<FK_PixelShaderStageCallback*> stagePixelShaderCallback;
		f32 * scenePixelShaderParameters;
		FK_SceneType sceneType;
		//gravity
		bool player1WasJumping;
		bool player2WasJumping;
		bool pausePlayer1;
		bool pausePlayer2;
		// move list panels
		FK_MoveListPanel* moveListPlayer1;
		FK_MoveListPanel* moveListPlayer2;
		bool showMoveListPlayer1;
		bool showMoveListPlayer2;
		// combo counters
		bool player1OldHitStun;
		bool player2OldHitStun;
		// continue counter
		s32 continueCounterSeconds;
		u32 continueCounterMS;
		u32 continueCounterDurationMS;
		u32 continuePressedButtons;
		/* variables concerning throws */
		FK_Move* player1ThrowAttemptMove;
		FK_Move* player2ThrowAttemptMove;
		bool AI1_defenseCheckAgainstThrowPerformed;
		bool AI2_defenseCheckAgainstThrowPerformed;
		bool skipThrowCheckPlayer1;
		bool skipThrowCheckPlayer2;
		s32 impactEffectCounter;
		u32 throwTimer;
		FK_Tutorial* pauseMenuTutorial;
		bool showTutorial;
		u32 lastTimeInputTutorial;
		s32 triggerRegenCounterMSPlayer1;
		s32 triggerRegenCounterMSPlayer2;
		std::vector<std::pair<FK_Character::FK_SceneProp*, IAnimatedMeshSceneNode*> > brokenObjectsProps;
		std::vector<FK_WorldCollisionCallbackProp*> brokenObjectCollisionCallbacks;
		bool needMeshSynchronization;
		// shaders
		std::vector<s32> characterMeshMaterialPlayer1;
		std::vector<s32> characterMeshMaterialPlayer2;
		// cinematic camera
		bool cinematicCameraActivePlayer1;
		bool cinematicCameraActivePlayer2;
		u32 counterAttackEffectCounterMs;
		u32 counterAttackEffectCooldownCounterMs;
		bool updateCounterAttackEffectFlag;
		// render target textures for performance improvement
		std::vector<video::ITexture*> renderTargetTextures;
		std::vector<core::vector2df> renderTargetTexturesScaleFactors;
		s32 renderTargetIndex = -1;
		s32 renderQualityBadFramesCounter = 0;
		s32 renderQualityGoodFramesCounter = 0;
		core::dimension2du oldRenderTargetResolution;
		core::dimension2du currentRenderTargetResolution;
	};
}
#endif