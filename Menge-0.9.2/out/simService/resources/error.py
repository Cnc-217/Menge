from simService import sims,redisServer

#捕获到异常的回调
def errorConnectionResetError(sim):
    redisServer.delete(sim.getPid())
    sims.stopSimulation(sim.getPid())