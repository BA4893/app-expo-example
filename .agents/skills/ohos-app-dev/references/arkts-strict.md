# ArkTS Strict Mode & Constraints

ArkTS is a statically typed language that imposes stricter rules than standard TypeScript to ensure high performance.

## ❌ Illegal / Forbidden Patterns

### Dynamic Typing & Shapes
- **`any` / `unknown`:** Forbidden. All types must be explicit and static.
  ```typescript
  let x: any = 1; // ❌ ERROR
  let y: number = 1; // ✅ CORRECT
  ```
- **Runtime Shape Changes:** You cannot add properties to objects at runtime.
  ```typescript
  const obj = new MyClass();
  obj.newProp = "later"; // ❌ ERROR
  ```

### Object Literals
- **Type Inference Limitations:** Do not rely on inline object literal types.
  ```typescript
  type Inline = { value: number };  // ❌ DISCOURAGED
  const bad: Inline = { value: 1 }; // ❌ ERROR: Fresh object literal requires explicit class/interface.
  ```
- **Method Context:** `this` context in object literals is restricted.
  ```typescript
  const obj = {
     value: 1,
     double() {
         return this.value * 2; // ❌ ERROR: `this` usage restricted here.
     }
  };
  ```

### Type Coercion
- **Unary `+`:** Do not use `+` to convert strings to numbers.
  ```typescript
  console.info(String(+"7")); // ❌ ERROR
  ```
- **Correct Parsing:**
  ```typescript
  const s: string = "7";
  const n: number = Number.parseInt(s); // ✅ CORRECT
  ```

### Type Safety
- **Strict Assignment:** Types are never implicitly converted during assignment.
  ```typescript
  let n: number = 1;
  n = "7"; // ❌ ERROR
  ```

## ✅ Best Practices
1. **Always use Classes/Interfaces:** Define your data structures explicitly.
2. **Explicit Returns:** Always declare return types for functions.
3. **Static Dispatch:** Avoid dynamic property access (`obj['prop']`).
