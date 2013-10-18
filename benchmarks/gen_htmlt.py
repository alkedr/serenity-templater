import string
import random

for i in range(10000):
	print(''.join(random.choice(string.ascii_lowercase + string.ascii_uppercase + string.digits) for x in range(random.randrange(3, 100))));
	print('$(data[' + str(random.randint(0, 999)) + '])');

