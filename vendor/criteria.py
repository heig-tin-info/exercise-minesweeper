import ruamel.yaml

yaml = ruamel.yaml.YAML() 

class Criteria():
    def __init__(self, filename='./criteria.yml'):        
        self.filename = filename
        with open(self.filename, 'r') as fp:
            self._data = yaml.load(fp)

    def _score(self):
        score = {
            'obtained': 0,
            'total': 0,
            'bonus': 0
        }

        def get_points(key, value=None, depth=None, context=None):
            if (key == '$points'):
                context['obtained'] += value[0]
                context['total'] += value[1]
            if (key == '$bonus'):
                context['obtained'] += value[0]
                context['bonus'] += value[1]

        self.walk(get_points, context=score)    

        return score

    def set_test(self, test, status):
        if status:
            self.set_success(test)
        else:
            self.set_failure(test)
            
    def set_success(self, test):
        def get(key, value=None, depth=None, context=None):
            if not value or not hasattr(value, 'keys'): 
                return 
            keys = value.keys()
            if not keys: 
                return
            if '$test' in keys and value['$test'] == context[0]:
                if '$points' in keys:
                    value['$points'][0] = value['$points'][1]
                    context[1] = True
                if '$bonus' in keys:
                    value['$bonus'][0] = value['$bonus'][1]
                    context[1] = True

        context = [test, False]
        self.walk(get, context=context)         
        if not context[0]:
            raise ValueError('Test not found')

    def set_failure(self, test):
        def get(key, value=None, depth=None, context=None):
            if not value or not hasattr(value, 'keys'): 
                return 
            keys = value.keys()
            if not keys: 
                return
            if '$test' in keys and value['$test'] == context[0]:
                if '$points' in keys:
                    value['$points'][0] = 0
                    context[1] = True
                if '$bonus' in keys:
                    value['$bonus'][0] = 0
                    context[1] = True

        context = [test, False]
        self.walk(get, context=context)         
        if not context[0]:
            raise ValueError('Test not found')

    @property
    def tests(self):
        tests = []

        def get_tests(key, value=None, depth=None, context=None):
            if (key == '$test'):
                context.append(value)

        self.walk(get_tests, context=tests)    

        return tests

    @property
    def score(self):
        score = self._score()
        return score['obtained'] / score['total'] * 5.0 + 1.0

    @property
    def total(self):
        score = self._score()
        return score['total']

    def walk(self, func, d=None, depth=0, context=None):
        if d is None : d = self._data
        for k,v in sorted(d.items(),key=lambda x: x[0]):
            if isinstance(v, dict):
                func(k, v, depth=depth, context=context)
                self.walk(func, v, depth+1, context=context)
            else:
                func(k, v, depth=depth, context=context)

    def save(self):
        """Save score in file"""
        with open(self.filename, 'w') as fp:
            yaml.dump(self._data, fp)
