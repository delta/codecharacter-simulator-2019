# Build base image
docker build -f docker/base/Dockerfile -t deltanitt/codecharacter-base-2019 .

# Build worker images
docker build -f docker/compiler/Dockerfile -t deltanitt/codecharacter-compiler-2019 .
docker build -f docker/runner/Dockerfile -t deltanitt/codecharacter-runner-2019 .

# Push images to dockerhub
docker push deltanitt/codecharacter-base-2019
docker push deltanitt/codecharacter-compiler-2019
docker push deltanitt/codecharacter-runner-2019
