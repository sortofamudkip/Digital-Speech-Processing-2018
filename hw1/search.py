# search.py
# ---------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
	"""
	This class outlines the structure of a search problem, but doesn't implement
	any of the methods (in object-oriented terminology: an abstract class).

	You do not need to change anything in this class, ever.
	"""

	def getStartState(self):
		"""
		Returns the start state for the search problem.
		"""
		util.raiseNotDefined()

	def isGoalState(self, state):
		"""
		  state: Search state

		Returns True if and only if the state is a valid goal state.
		"""
		util.raiseNotDefined()

	def getSuccessors(self, state):
		"""
		  state: Search state

		For a given state, this should return a list of triples, (successor,
		action, stepCost), where 'successor' is a successor to the current
		state, 'action' is the action required to get there, and 'stepCost' is
		the incremental cost of expanding to that successor.
		"""
		util.raiseNotDefined()

	def getCostOfActions(self, actions):
		"""
		 actions: A list of actions to take

		This method returns the total cost of a particular sequence of actions.
		The sequence must be composed of legal moves.
		"""
		util.raiseNotDefined()


def tinyMazeSearch(problem):
	"""
	Returns a sequence of moves that solves tinyMaze.  For any other maze, the
	sequence of moves will be incorrect, so only use this for tinyMaze.
	"""
	from game import Directions
	s = Directions.SOUTH
	w = Directions.WEST
	return  [s, s, w, s, w, w, s, w]

def depthFirstSearch(problem):
	node = problem.getStartState() # state
	if problem.isGoalState(node):
		return [] # don't do anything
	frontier = util.Stack()
	frontier.push(node)
	explored = set()
	parent_map = {}
	while True:
		# check if the thingy is empty?
		node = frontier.pop() #retrieve first node
		# print "chose", node
		if problem.isGoalState(node): #solution found!
			solution = []
			curr = (node, None)
			while True:
				if curr[1] is not None:
					solution.append(curr[1])
				if curr[0] in parent_map:
					curr = parent_map[curr[0]]
				else:
					break
			return solution[::-1]
		explored.add(node)
		children = problem.getSuccessors(node)
		for child in children:
			child_node = child[0]
			if child_node not in explored :
				parent_map[child_node] = (node, child[1]) # save action as path
				# print "at ", node, ": parent_map[", child_node, "] = (", node, child[1], ")" 
				frontier.push(child_node)

def breadthFirstSearch(problem):
	"""Search the shallowest nodes in the search tree first."""
	node = problem.getStartState() # state
	if problem.isGoalState(node):
		return [] # don't do anything
	frontier = util.Queue()
	frontier.push(node)
	explored = set()
	parent_map = {}
	while True:
		# check if the thingy is empty?
		node = frontier.list[0] #retrieve first node
		del frontier.list[0]
		# print "chose", node
		if problem.isGoalState(node): #solution found!
			solution = []
			curr = (node, None)
			while True:
				if curr[1] is not None:
					solution.append(curr[1])
				if curr[0] in parent_map:
					curr = parent_map[curr[0]]
				else:
					break
			return solution[::-1]
		explored.add(node)
		children = problem.getSuccessors(node)
		for child in children:
			child_node = child[0]
			if child_node not in explored and child_node not in frontier.list:
				parent_map[child_node] = (node, child[1]) # save action as path
				frontier.list.append(child_node)

def in_heap(state, heap):
	for i in range(len(heap)):
		if state == heap[i][2][1]:
			return i, heap[i][2][2]
	return -1, -1


def uniformCostSearch(problem):
	node = problem.getStartState() # state
	if problem.isGoalState(node):
		return [] # don't do anything
	frontier = util.PriorityQueue()
	frontier.push(([], node, 0), 0) # insert into PQ
	explored = set()
	# ^^^ don't touch this
	while True:
		# check if the thingy is empty? I guess not.
		path, node, cost = frontier.pop() #retrieve first node
		# print "chose", node
		if problem.isGoalState(node): #solution found!
			return path
		explored.add(node)
		children = problem.getSuccessors(node) # listof (node, action, cost)
		for child in children:
			child_node = child[0]
			new_cost = cost + child[2]
			in_heap_index, possible_cost = in_heap(child_node, frontier.heap)
			if child_node not in explored and in_heap_index == -1:
				new_path = [x for x in path]; new_path.append(child[1])
				frontier.push((new_path, child_node, new_cost), new_cost)
				# print "at {}, inserted {} (cost = {})".format(node, child_node, new_cost)
				# util.manhattanDistance(child_node, ) <-- might need this later
			elif in_heap_index >= 0 and new_cost < possible_cost:
				new_path = [x for x in path]; new_path.append(child[1])
				del frontier.heap[in_heap_index]
				frontier.push((new_path, child_node, new_cost), new_cost)

def nullHeuristic(state, problem=None):
	"""
	A heuristic function estimates the cost from the current state to the nearest
	goal in the provided SearchProblem.  This heuristic is trivial.
	"""
	return 0

def aStarSearch(problem, heuristic=nullHeuristic):
	"""Search the node that has the lowest combined cost and heuristic first."""
	node = problem.getStartState() # state
	if problem.isGoalState(node):
		return [] # don't do anything
	frontier = util.PriorityQueue()
	estimate = heuristic(node, problem)
	frontier.push((node, [], estimate), estimate) # insert into PQ
	explored = set()
	# ^^^ don't touch this
	while True:
		# check if the thingy is empty? I guess not.
		node, path, orig_cost = frontier.pop() #retrieve first node
		# print "chose", node
		if problem.isGoalState(node): #solution found!
			return path

		if node not in explored:
			explored.add(node)
			children = problem.getSuccessors(node) # listof (node, action, cost)
			for child in children:
				new_path = list(path); new_path.append(child[1])

				estimate = heuristic(child[0], problem)
				new_cost = problem.getCostOfActions(new_path)+estimate
				frontier.push((child[0], new_path, new_cost), new_cost)

# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
