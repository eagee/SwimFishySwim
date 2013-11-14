// INodeWaypoints.h
// Used by pathfinding code (see: TiledMaps.cpp) to determine which tile nodes have been traversed by ID and how often they have been traversed.
// Author(s): Eagan Rackley
#ifndef INODE_HISTORY_H
#define INODE_HISTORY_H

///<summary>
/// Used to store a map containing all of the node history ...
///</summary>
typedef std::deque<Vector> NodeWaypoints;
typedef NodeWaypoints::iterator NodeWaypointsIterator;

///<summary>
/// Specifies that the sprite does not currently have a target it is attempting to reach ...
///</summary>
#define NO_WAYPOINT_SPECIFIED {0.0f, 0.0f, 0.0f}

///<summary>
/// Used by pathfinding code (see: TiledMaps.cpp) to determine which tile nodes have been traversed by ID and how often they have been traversed.
///</summary>
class INodeWaypoints
{ 
	public:
		INodeWaypoints(){}
		
		virtual ~INodeWaypoints(){}
		
		///<summary>
		/// Adds a waypoint to our list of waypoints to access
		///</summary>			
		virtual void addWaypoint(Vector waypoint) = 0;

		///<summary>
		/// Gets the current waypoint vector that the INodeWaypoints implementation is targeting
		///</summary>			
		virtual Vector getCurrentWaypoint() = 0;

		///<summary>
		/// clears the history of waypoint data ...
		///</summary>	
		virtual void clearWaypoints() = 0;						
};

#endif
