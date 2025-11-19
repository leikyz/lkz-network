#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace Constants 
{
    // ----- Engine -----

	inline constexpr int SERVER_PORT = 5555;    // Default server port
	inline const char* const SERVER_IP = "127.0.0.1"; // Default server IP address
	inline constexpr float FIXED_DELTA_TIME = 0.02f; // Fixed update timestep (50 updates per second)
	inline const char* const APP_NAME = "LKZ Network"; // Application name
	inline const size_t NETWORK_BUFFER_SIZE = 1024; // Size of the network buffer for sending/receiving data

     // ----- Mathematical -----

	inline constexpr double PI = 3.141592653589793; // Pi constant	

    // ----- Crowd Inialization -----

    inline constexpr int MAX_AGENTS = 1000;             // Maximum number of agents the crowd manager can handle at once.
    inline constexpr float MAX_AGENT_RADIUS = 0.5f;    // The maximum radius any agent in the crowd can have. This is used internally by DetourCrowd.

    // ----- Default Agent Settings -----

    inline constexpr float AGENT_RADIUS = 0.1f;       // The physical radius of a typical AI agent.
    inline constexpr float AGENT_HEIGHT = 1.8f;      // The height of an agent (e.g., for humanoid navigation).
    inline constexpr float AGENT_MAX_ACCELERATION = 1.0f; // Maximum acceleration allowed for an agent.
    inline constexpr float AGENT_MAX_SPEED = 0.4f;       // Maximum movement speed of an agent (units per second).

    // ----- Avoidance & Collision -----

    inline constexpr float AGENT_COLLISION_QUERY_RANGE = AGENT_RADIUS * 4.0f;      // Distance around the agent used to query nearby agents for collision avoidance.
    inline constexpr float AGENT_PATH_OPTIMIZATION_RANGE = AGENT_RADIUS * 15.0f;  // How far the agent will look ahead to optimize its path (avoid walls, etc.).
    inline constexpr int AGENT_OBSTACLE_AVOIDANCE_TYPE = 3;                      // Quality level of obstacle avoidance: 0 = Low, 1 = Medium, 2 = Good, 3 = High (default: 3)
    inline constexpr float AGENT_SEPARATION_WEIGHT = 2.0f;                      // How strongly agents try to stay away from each other.

    // ----- Update Agent Flags -----
        
    inline constexpr unsigned int AGENT_UPDATE_FLAGS =  // Bitmask of behavior flags used by DetourCrowd. These control how the crowd updates each agent.
        
        (1 << 0) |  // DT_CROWD_ANTICIPATE_TURNS: anticipate turns for smoother paths
        (1 << 1) |  // DT_CROWD_OPTIMIZE_VIS: optimize visibility to nearby corners
        (1 << 2) |  // DT_CROWD_OPTIMIZE_TOPO: optimize path topology
        (1 << 3) |  // DT_CROWD_OBSTACLE_AVOIDANCE: enable dynamic avoidance
        (1 << 4);   // DT_CROWD_SEPARATION: enable separation between agents

	// ----- Navigation Query Filter -----

    inline constexpr int AGENT_QUERY_FILTER_TYPE = 0; // Index of the filter used for navigation queries. Must match the one defined in the NavMesh setup.

	// ----- ECS -----

	inline constexpr float PLAYER_MOVE_SPEED = 2.0f; // Player movement speed in units per second. 0.2 crea | 1.0 home
    inline constexpr float PLAYER_WALK_SPEED_MULTIPLICATOR = 1.0;
    inline constexpr float PLAYER_RUN_SPEED_MULTIPLICATOR = 2.0;
    inline constexpr float PLAYER_WALK_ARMED_SPEED_MULTIPLICATOR = 0.7;
    inline constexpr float PLAYER_RUN_ARMED_SPEED_MULTIPLICATOR = 1.5;
    inline constexpr float PLAYER_AIM_SPEED_MULTIPLICATOR = 0.5;
	inline constexpr float PLAYER_MOVE_THRESHOLD = 0.02f; // Minimum distance change to trigger a position update.
	inline constexpr int PLAYER_MESSAGE_RATE = 5.0f; 	 // Send player position updates every N ticks.

	inline constexpr int AI_MESSAGE_RATE = 0.2; // Send AI position updates every N ticks.    
	inline constexpr float AI_REPATH_RATE = 1.0f; // AI repath interval in seconds.
	inline constexpr float AI_STOP_DISTANCE = 2.0f; // Distance at which AI stops moving towards target.
	inline constexpr float AI_STOP_DISTANCE_SQ = AI_STOP_DISTANCE * AI_STOP_DISTANCE; // Squared stop distance for efficiency.
}

#endif // CONSTANTS_HPP
