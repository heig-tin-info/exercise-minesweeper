from blessings import Terminal
from collections import OrderedDict, defaultdict

class OrderedMeta(type):
    @classmethod
    def __prepare__(metacls, name, bases): 
        return OrderedDict()

    def __new__(cls, name, bases, clsdict):
        c = type.__new__(cls, name, bases, clsdict)
        c._orderedKeys = clsdict.keys()
        return c

def category(category):
    def decorator(func):
        def wrapper(*args, **kwargs):
            func(*args, **kwargs)
        wrapper._category = category            
        return wrapper
    decorator._category = category    
    return decorator

def AssertTrue(condition, message=None):
    pass

class Test(metaclass=OrderedMeta):
    def __init__(self):
        self.t = Terminal()
        self.tests = 1
        self.errors = 0

    def run(self):
        for category, tests in self.collect().items():
            for name, test in tests:
                if test.__doc__:
                    print(name, self.t.bold(test.__doc__))
                test()
                print("")

        if self.errors == 0:
            print(f"Successfully executed {self.tests} tests\n\nOK")
            sys.exit(0)
        else:
            s = "s" if self.errors > 1 else ""
            print(
                f"Successfully executed {self.tests - self.errors} of {self.tests} tests, {self.errors} error{s}\n\nFAIL")
            sys.exit(1)        

    def collect(self):
        tests = defaultdict(list)
        for method in self._orderedKeys:
            if not method.startswith('test_'): continue
            func = getattr(self, method)
            category = func._category if hasattr(func, '_category') else 'Default'
            tests[func._category].append((method, func))
        return tests

    def test(self, assertion, message=None, error_message=None):
        if message:
            print(f"{self.tests}. {message}...", end='', flush=True)
        else:
            print(f"{self.tests}. ", end='', flush=True)
        if assertion:
            print(self.t.green(" pass"), flush=True)
        else:
            self.errors += 1
            print(self.t.red(" fail"), flush=True)
            if error_message:
                print(self.t.red("   " + error_message))
        self.tests += 1
