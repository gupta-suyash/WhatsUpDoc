class CreateNodes < ActiveRecord::Migration
  def change
    create_table :nodes do |t|
      t.integer :type
      t.integer :version
      t.integer :time
      t.string :data

      t.timestamps null: false
    end
  end
end
