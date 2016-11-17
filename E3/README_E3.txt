SEMAPHORES

Test[0]: [testInvalidSemSignal] SEM_INIT(invalid semaphores id: negative & out of range id) :sem_init
Test[1]: [testInitInvalidSem] SEM_INIT(an already initialized semaphore) : sem_init,sem_destroy
Test[2]: [testDestroyInvSem] SEM_DESTROY(invalid semaphore id: negative & out of range id) : sem_destroy
Test[3]: [testDestroyNotInitSem] SEM_DESTROY(an uninitialized semaphore) : sem_destroy
Test[4]: [testDestroyNotOwner] SEM_DESTROY(not owner) : fork, sem_init, sem_destroy, sem_wait, sem_signal, nice, exit
Test[5]: [testDestroyExit] EXIT DESTROY OWNED SEMS : fork, exit, sem_init, sem_signal, sem_wait
Test[6]: [testDoNotDestroyExit] EXIT DO NOT DESTROY NOT OWNED SEMS : fork, exit, sem_init, sem_signal
Test[7]: [testInvalidSemSignal] SEM_SIGNAL(invalid semaphore id: negative & out of range id) : sem_signal
Test[8]: [testUnitializedSemSignal] SEM_SIGNAL(not initialized semaphore) : sem_signal
Test[9]: [testReuseSem] TEST SEMAPHORE REUSE : sem_init,sem_destroy
Test[10]: [testInvalidSemWait] SEM_WAIT(invalid semaphore id: negative & out of range id) : sem_wait, fork, getpid
Test[11]: [testUnitializedSemWait] SEM_WAIT(not initialized semaphore) : sem_wait
Test[12]: [testSignalDestroyed] SEM_SIGNAL(destroyed semaphore) : sem_signal,sem_init,sem_wait, sem_destroy, getpid, fork, exit
Test[13]: [testSemFunctionality2] SEM_WAIT RETURN VALUE ON SUCCESS : sem_signal,sem_init,sem_wait, sem_destroy, getpid, fork, exit
Test[14]: [testDestroyBusySem1] SEM_DESTROY(busy semaphore) : sem_wait, sem_signal, sem_init, sem_destroy,nice
Test[15]: [testDestroyBusySem2] SEM_DESTROY(busy semaphore):unblock sem_wait : fork, sem_wait, sem_signal, sem_init, sem_destroy,nice
Test[16]: [testBlockManyProcess] SYNCHRONIZING SEVERAL PROCESSES : sem_signal,sem_init,sem_wait, sem_destroy, fork, exit


CLONE
Test[0]: [testcloneInvalidFunction] clone(invalid function address)
Test[1]: [testcloneInvalidStack] clone(invalid stack address) : exit
Test[2]: [testcloneOk] clone(OK) : exit
Test[3]: [testcloneSharesMem] clone()shares memory : exit, sem_init, sem_wait, sem_signal, sem_destroy
Test[4]: [testcloneNoSharesMem] fork()does not share memory : fork, exit, sem_init, sem_wait, sem_signal, sem_destroy
Test[5]: [testcloneManyThreads] clone many threads (single) : exit, sem_init, sem_wait, sem_signal, sem_destroy
Test[6]: [testcloneMaxThreas] clone maximum threads  : exit, sem_init, sem_wait, sem_signal, sem_destroy
Test[7]: [testcloneManyThreads2] clone many threads (maximum)  : exit, sem_init, sem_wait, sem_signal, sem_destroy
Test[8]: [testcloneForkAfterClone] fork after clone  : exit, fork, sem_init, sem_wait, sem_signal, sem_destroy
Test[9]: [testcloneForkInClone]fork inside clone  : exit, fork, sem_init, sem_wait, sem_signal, sem_destroy

	
