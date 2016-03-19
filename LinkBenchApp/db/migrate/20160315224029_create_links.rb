class CreateLinks < ActiveRecord::Migration
  def change
    create_table :links do |t|
      t.integer :id1
      t.integer :id2
      t.integer :linktype
      t.integer :visibility
      t.string :data
      t.integer :time
      t.integer :version

      t.timestamps null: false
    end
  end
end
