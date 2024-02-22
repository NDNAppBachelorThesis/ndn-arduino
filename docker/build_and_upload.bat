@echo off

echo "Login into container registry"
docker login -u derteufelqwe
echo "Login successful."

echo "Building image. This may take a while..."
docker build -t derteufelqwe/ndn-nfd:latest .
echo "Build completed. Pushing to registry..."

docker push derteufelqwe/ndn-nfd:latest
echo "Upload done. Visit https://github.com/orgs/NDNAppBachelorThesis/packages"
