#include <PackageQueue.H>
#include <iostream>

Package::Package()
{
    databuf = nullptr;
    bufSize = 0;
    source = 0;
    destination = 0;
    completed = false;
    notified = false;
    served = false;
    request = 0;
    packageLock= PTHREAD_MUTEX_INITIALIZER;
}

Package::~Package()
{
    //if(databuf) free(databuf);
    if(databuf!= nullptr) upcxx::deallocate(databuf);
}

Package::Package(int size)
{
    //databuf = new double[size];
    databuf = upcxx::allocate(size);
    bufSize = size;
    source = 0;
    destination = 0;
    completed = false;
    notified = false;
    served = false;
    request = 0;
    packageLock= PTHREAD_MUTEX_INITIALIZER;
}

Package::Package(int src, int dest)
{
    bufSize = 0;
    source = src;
    destination = dest;
}

Package::Package(int src, int dest, int size)
{
    source = src;
    destination = dest;
    //databuf = new double[size];
    databuf = upcxx::allocate(size);
    bufSize = size;
    source = 0;
    destination = 0;
    completed = false;
    notified = false;
    served = false;
    request = 0;
    packageLock= PTHREAD_MUTEX_INITIALIZER;
}

void Package::setPackageSource(int src)
{
    source = src;
}

void Package::setPackageDestination(int dest)
{
    destination = dest;
}

void Package::completeRequest(void)
{
    pthread_mutex_lock(&packageLock);
    completed = true;
    pthread_mutex_unlock(&packageLock);
}

void Package::completeRequest(bool lockIgnore)
{
    if(!lockIgnore)pthread_mutex_lock(&packageLock);
    completed = true;
    if(!lockIgnore)pthread_mutex_unlock(&packageLock);
}

bool Package::checkRequest(void)
{
    return completed;
}  

void Package::generatePackage(int size)
{
    //databuf = new double[size];
    databuf = upcxx::allocate(size);
    bufSize = size;
    source = 0;
    destination = 0;
    completed = false;
    notified = false;
    served = false;
    request = 0;
    packageLock= PTHREAD_MUTEX_INITIALIZER;
}

PackageQueue::PackageQueue()
{
    n = 0;
    front = 0;
    rear = 0;
    prear = -1;
    max_size=perilla::MSG_QUEUE_DEFAULT_MAXSIZE;
    queueLock= PTHREAD_MUTEX_INITIALIZER;;
}

int PackageQueue::queueSize(void)
{
    int size;
    pthread_mutex_lock(&queueLock);
    size = n;
    pthread_mutex_unlock(&queueLock);
    return size;
}

int PackageQueue::queueSize(bool lockIgnore)
{
    int size;
    if(!lockIgnore)pthread_mutex_lock(&queueLock);
    size = n;
    if(!lockIgnore)pthread_mutex_unlock(&queueLock);
    return size;
}

void PackageQueue::enqueue(Package* package)
{
    pthread_mutex_lock(&queueLock);
    buffer[rear] = package;
    prear = rear;    
    rear = (rear+1)%perilla::MSG_QUEUE_DEFAULT_MAXSIZE;
    n++;
    pthread_mutex_unlock(&queueLock);
}

void PackageQueue::enqueue(Package* package, bool lockIgnore)
{
    if(!lockIgnore)pthread_mutex_lock(&queueLock);
    buffer[rear] = package;
    prear = rear;
    rear = (rear+1)%perilla::MSG_QUEUE_DEFAULT_MAXSIZE;
    n++;
    if(!lockIgnore)pthread_mutex_unlock(&queueLock);
}

Package* PackageQueue::dequeue(void)
{
    Package* package = 0;
    pthread_mutex_lock(&queueLock);
    package = buffer[front];
    front = (front+1)%perilla::MSG_QUEUE_DEFAULT_MAXSIZE;
    n--;
    pthread_mutex_unlock(&queueLock);
    return package;
}

Package* PackageQueue::dequeue(bool lockIgnore)
{
    lockIgnore = false;
    Package* package = 0;
    if(!lockIgnore)pthread_mutex_lock(&queueLock);
    if(n<0)
	std::cout<< "Q size " << n << " front " << front <<std::endl;
    package = buffer[front];
    front = (front+1)%perilla::MSG_QUEUE_DEFAULT_MAXSIZE;
    n--;
    if(!lockIgnore)pthread_mutex_unlock(&queueLock);
    return package;
}

Package* PackageQueue::getRear(void)
{
    Package* package = 0;
    pthread_mutex_lock(&queueLock);
    package = buffer[prear];
    pthread_mutex_unlock(&queueLock);
    return package;
}
Package* PackageQueue::getRear(bool lockIgnore)
{
    Package* package = 0;
    if(!lockIgnore)pthread_mutex_lock(&queueLock);
    package = buffer[prear];
    if(!lockIgnore)pthread_mutex_unlock(&queueLock);
    return package;
}

Package* PackageQueue::getFront(void)
{
    Package* package = 0;
    pthread_mutex_lock(&queueLock);
    package = buffer[front];
    pthread_mutex_unlock(&queueLock);
    return package;
}

Package* PackageQueue::getFront(bool lockIgnore)
{
    Package* package = 0;
    if(!lockIgnore) pthread_mutex_lock(&queueLock);
    package = buffer[front];
    if(!lockIgnore) pthread_mutex_unlock(&queueLock);
    return package;
}

void PackageQueue::emptyQueue(){
    while(n){
        Package* p= dequeue(true);
        delete p;
    }
}

PackageQueue::~PackageQueue()
{
    emptyQueue();
}

